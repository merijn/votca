/*
 *            Copyright 2009-2019 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
/// For an earlier history see ctp repo commit
/// 77795ea591b29e664153f9404c8655ba28dc14e9

#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <votca/xtp/progressobserver.h>
#include <votca/xtp/qmthread.h>

using boost::format;

namespace votca {
namespace xtp {

template <typename JobContainer>
ProgObserver<JobContainer>::Job *ProgObserver<JobContainer>::RequestNextJob(
    QMThread &thread) {

  _lockThread.Lock();
  Job *jobToProc = nullptr;

  XTP_LOG(logDEBUG, thread.getLogger()) << "Requesting next job" << std::flush;

  // NEED NEW CHUNK?
  if (_nextjit == _jobsToProc.end() && _moreJobsAvailable) {
    SyncWithProgFile(thread);
    _nextjit = _jobsToProc.begin();
    if (_nextjit == _jobsToProc.end()) {
      _moreJobsAvailable = false;
      XTP_LOG(logDEBUG, thread.getLogger())
          << "Sync did not yield any new jobs." << std::flush;
    }
  }

  // JOBS EATEN ALL UP?
  if (_nextjit == _jobsToProc.end()) {
    if (_maxJobs == _startJobsCount) {
      XTP_LOG(logDEBUG, thread.getLogger())
          << "Next job: ID = - (reached maximum for this process)"
          << std::flush;
    } else {
      XTP_LOG(logDEBUG, thread.getLogger())
          << "Next job: ID = - (none available)" << std::flush;
    }
  }
  // TAKE A BITE
  else {
    jobToProc = *_nextjit;
    ++_nextjit;
    XTP_LOG(logDEBUG, thread.getLogger())
        << "Next job: ID = " << jobToProc->getId() << std::flush;
  }

  if (!thread.isMaverick() && jobToProc != nullptr) {
    int idx = jobToProc->getId();
    int frac = (_jobs.size() >= 10) ? 10 : _jobs.size();
    int rounded = int(double(_jobs.size()) / frac) * frac;
    int tenth = rounded / frac;
    if (idx % tenth == 0) {
      double percent = double(idx - 1) / rounded * 100 + 0.5;
      std::cout << (format("=> [%1$2.0f%%] ") % percent).str() << std::flush;
    }
  }

  _lockThread.Unlock();
  return jobToProc;
}

template <typename JobContainer>
void ProgObserver<JobContainer>::ReportJobDone(Job &job, Result &res,
                                               QMThread &thread) {
  _lockThread.Lock();
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Reporting job results" << std::flush;
  // RESULTS, TIME, HOST
  job.SaveResults(res);
  job.setTime(GenerateTime());
  job.setHost(GenerateHost(thread));
  // PRINT PROGRESS BAR
  _jobsReported += 1;
  if (!thread.isMaverick())
    std::cout << std::endl << thread.getLogger() << std::flush;
  _lockThread.Unlock();
  return;
}

template <typename JobContainer>
std::string ProgObserver<JobContainer>::GenerateHost(QMThread &thread) {
  char host[128];
  (void)gethostname(host, sizeof host);
  pid_t pid = getpid();
  return (format("%1$s:%2$d") % host % pid).str();
}

template <typename JobContainer>
std::string ProgObserver<JobContainer>::GenerateTime() {
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  return (format("%1$s") % now.time_of_day()).str();
}

template <typename JobContainer>
void ProgObserver<JobContainer>::SyncWithProgFile(QMThread &thread) {

  // INTERPROCESS FILE LOCKING (THREAD LOCK IN ::RequestNextJob)
  this->LockProgFile(thread);

  std::string progFile = _progFile;
  std::string progBackFile = _progFile + "~";
  std::string tabFile = progFile;
  boost::algorithm::replace_last(tabFile, ".xml", ".tab");
  if (tabFile == progFile) tabFile += ".tab";
  std::string tabBackFile = tabFile + "~";

  // LOAD EXTERNAL JOBS FROM SHARED XML & UPDATE INTERNAL JOBS
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Update internal structures from job file" << std::flush;
  JobContainer jobs_ext = LOAD_JOBS(progFile);
  UPDATE_JOBS(jobs_ext, _jobs, GenerateHost(thread));

  // GENERATE BACK-UP FOR SHARED XML
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Create job-file back-up" << std::flush;
  WRITE_JOBS(_jobs, progBackFile, "xml");
  WRITE_JOBS(_jobs, tabBackFile, "tab");

  // ASSIGN NEW JOBS IF AVAILABLE
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Assign jobs from stack" << std::flush;
  _jobsToProc.clear();

  int cacheSize = _cacheSize;
  while (int(_jobsToProc.size()) < cacheSize) {
    if (_metajit == _jobs.end() || _startJobsCount == _maxJobs) break;

    bool startJob = false;

    // Start if job available or restart patterns matched
    if ((_metajit->isAvailable()) ||
        (_restartMode && _restart_stats.count(_metajit->getStatusStr())) ||
        (_restartMode && _restart_hosts.count(_metajit->getHost())))
      startJob = true;

    if (startJob) {
      _metajit->Reset();
      _metajit->setStatus("ASSIGNED");
      _metajit->setHost(GenerateHost(thread));
      _metajit->setTime(GenerateTime());
      _jobsToProc.push_back(&*_metajit);
      _startJobsCount += 1;
    }

    ++_metajit;
  }

  // UPDATE PROGRESS STATUS FILE
  WRITE_JOBS(_jobs, progFile, "xml");
  WRITE_JOBS(_jobs, tabFile, "tab");

  // RELEASE PROGRESS STATUS FILE
  this->ReleaseProgFile(thread);
  return;
}

template <typename JobContainer>
void ProgObserver<JobContainer>::LockProgFile(QMThread &thread) {
  _flock = std::unique_ptr<boost::interprocess::file_lock>(
      new boost::interprocess::file_lock(_lockFile.c_str()));
  _flock->lock();
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Imposed lock on " << _lockFile << std::flush;
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Sleep ... " << _lockFile << std::flush;
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Wake up ... " << _lockFile << std::flush;
}

template <typename JobContainer>
void ProgObserver<JobContainer>::ReleaseProgFile(QMThread &thread) {
  _flock->unlock();
  XTP_LOG(logDEBUG, thread.getLogger())
      << "Releasing " << _lockFile << ". " << std::flush;
}

template <typename JobContainer>
void ProgObserver<JobContainer>::InitCmdLineOpts(
    const boost::program_options::variables_map &optsMap) {

  _lockFile = optsMap["file"].as<std::string>();
  _cacheSize = optsMap["cache"].as<int>();
  _maxJobs = optsMap["maxjobs"].as<int>();
  std::string restartPattern = optsMap["restart"].as<std::string>();

  // restartPattern = e.g. host(pckr124:1234) stat(FAILED)
  boost::algorithm::replace_all(restartPattern, " ", "");
  if (restartPattern == "")
    _restartMode = false;
  else
    _restartMode = true;

  std::vector<std::string> split;
  tools::Tokenizer toker(restartPattern, "(,)");
  toker.ToVector(split);

  std::string category = "";
  for (unsigned int i = 0; i < split.size(); ++i) {

    if (split[i] == "host" || split[i] == "stat")
      category = split[i];

    else if (category == "host")
      _restart_hosts[split[i]] = true;
    else if (category == "stat") {
      if (split[i] == "ASSIGNED" || split[i] == "COMPLETE")
        std::cout << "Restart if status == " << split[i]
                  << "? Not necessarily a good idea." << std::endl;
      _restart_stats[split[i]] = true;
    }

    else
      throw std::runtime_error(
          "Restart pattern ill-defined, format is"
          "[host([HOSTNAME:PID])] [stat([STATUS])]");
  }
  return;
}

template <typename JobContainer>
void ProgObserver<JobContainer>::InitFromProgFile(std::string progFile,
                                                  QMThread &thread) {

  _progFile = progFile;
  _jobsReported = 0;

  XTP_LOG(logINFO, thread.getLogger()) << "Job file = '" << _progFile << "', ";
  XTP_LOG(logINFO, thread.getLogger()) << "lock file = '" << _lockFile << "', ";
  XTP_LOG(logINFO, thread.getLogger())
      << "cache size =  " << _cacheSize << std::flush;

  XTP_LOG(logINFO, thread.getLogger())
      << "Initialize jobs from " << progFile << std::flush;
  XTP_LOG(logINFO, thread.getLogger()) << "Lock & load " << std::flush;

  // LOCK, READ INTO XML
  this->LockProgFile(thread);

  // ... Clear container
  _jobs.clear();

  // ... Load new, set availability bool
  _jobs = LOAD_JOBS(progFile);
  _metajit = _jobs.begin();
  WRITE_JOBS(_jobs, progFile + "~", "xml");
  XTP_LOG(logINFO, thread.getLogger())
      << "Registered " << _jobs.size() << " jobs." << std::flush;
  if (_jobs.size() > 0)
    _moreJobsAvailable = true;
  else
    _moreJobsAvailable = false;

  // SUMMARIZE OBSERVER VARIABLES: RESTART PATTERN, CACHE, LOCK FILE
  if (_restartMode && _restart_hosts.size()) {
    std::string infostr = "Restart if host == ";
    for (const std::pair<std::string, bool> &host : _restart_hosts) {
      infostr += host.first + " ";
    }
    XTP_LOG(logINFO, thread.getLogger()) << infostr << std::flush;
  }
  if (_restartMode && _restart_stats.size()) {
    std::string infostr = "Restart if stat == ";
    for (const std::pair<std::string, bool> &host : _restart_hosts) {
      infostr += host.first + " ";
    }
    XTP_LOG(logINFO, thread.getLogger()) << infostr << std::flush;
  }

  // RELEASE PROGRESS FILE
  this->ReleaseProgFile(thread);
  return;
}

std::vector<Job> LOAD_JOBS(const std::string &job_file) {

  tools::Property xml;
  load_property_from_xml(xml, job_file);

  std::vector<tools::Property *> jobProps = xml.Select("jobs.job");
  std::vector<Job> jobs;
  jobs.reserve(jobProps.size());
  for (tools::Property *prop : jobProps) {
    jobs.push_back(Job(*prop));
  }

  return jobs;
}

void WRITE_JOBS(const std::vector<Job> &jobs, const std::string &job_file,
                std::string fileformat) {
  std::ofstream ofs;
  ofs.open(job_file.c_str(), std::ofstream::out);
  if (!ofs.is_open()) {
    throw std::runtime_error("Bad file handle: " + job_file);
  }
  if (fileformat == "xml") ofs << "<jobs>" << std::endl;
  for (auto &job : jobs) {
    if (fileformat == "tab" && !job.isComplete()) continue;
    job.ToStream(ofs, fileformat);
  }
  if (fileformat == "xml") ofs << "</jobs>" << std::endl;

  ofs.close();
  return;
}

void UPDATE_JOBS(const std::vector<Job> &from, std::vector<Job> &to,
                 std::string thisHost) {
  std::vector<Job>::iterator it_int;
  std::vector<Job>::const_iterator it_ext;

  if (to.size() != from.size())
    throw std::runtime_error("Progress file out of sync (::size), abort.");

  for (it_int = to.begin(), it_ext = from.begin(); it_int != to.end();
       ++it_int, ++it_ext) {
    Job &job_int = *it_int;
    const Job &job_ext = *it_ext;
    if (job_int.getId() != job_ext.getId())
      throw std::runtime_error("Progress file out of sync (::id), abort.");
    if (job_ext.hasHost() && job_ext.getHost() != thisHost)
      job_int.UpdateFrom(job_ext);
  }

  return;
}

// REGISTER
template class ProgObserver<std::vector<Job> >;

}  // namespace xtp
}  // namespace votca
