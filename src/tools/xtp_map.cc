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

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "votca/tools/application.h"
#include <votca/csg/topologyreader.h>
#include <votca/csg/trajectoryreader.h>
#include <votca/csg/trajectorywriter.h>
#include <votca/tools/globals.h>
#include <votca/xtp/Md2QmEngine.h>
#include <votca/xtp/statesaversqlite.h>
#include <votca/xtp/version.h>

#include <votca/xtp/atom.h>
#include <votca/xtp/segment.h>
#include <votca/xtp/topology.h>

using namespace std;

namespace CSG = votca::csg;
namespace XTP = votca::xtp;
namespace TOOLS = votca::tools;

class XtpMap : public TOOLS::Application {

 public:
  string ProgramName() { return "xtp_map"; }
  void HelpText(ostream &out) { out << "Generates QM|MD topology" << endl; }
  void ShowHelpText(std::ostream &out);

  void Initialize();
  bool EvaluateOptions();
  void Run();
  void Save();

 protected:
  TOOLS::Property _options;
  CSG::Topology _mdtopol;
  XTP::Topology _qmtopol;

  XTP::Md2QmEngine _md2qm;
  XTP::StateSaverSQLite _statsav;
  string _outdb;
};

namespace propt = boost::program_options;

void XtpMap::Initialize() {

  TOOLS::Application::Initialize();

  CSG::TrajectoryWriter::RegisterPlugins();
  CSG::TrajectoryReader::RegisterPlugins();
  CSG::TopologyReader::RegisterPlugins();

  AddProgramOptions()("topology,t", propt::value<string>(), "  topology");
  AddProgramOptions()("coordinates,c", propt::value<string>(),
                      "  coordinates or trajectory");
  AddProgramOptions()("segments,s", propt::value<string>(),
                      "  definition of segments and fragments");
  AddProgramOptions()("file,f", propt::value<string>(), "  state file");
}

bool XtpMap::EvaluateOptions() {

  CheckRequired("topology", "Missing topology file");
  CheckRequired("segments", "Missing segment definition file");
  CheckRequired("coordinates", "Missing trajectory input");
  CheckRequired("file", "Missing state file");

  return 1;
}

void XtpMap::Run() {

  std::string name = ProgramName();
  if (VersionString() != "") name = name + ", version " + VersionString();
  XTP::HelpTextHeader(name);

  // +++++++++++++++++++++++++++++++++++++ //
  // Initialize MD2QM Engine and SQLite Db //
  // +++++++++++++++++++++++++++++++++++++ //

  _outdb = _op_vm["file"].as<string>();
  _statsav.Open(_outdb, false);
  int frames_in_db = _statsav.FramesInDatabase();
  if (frames_in_db > 0) {
    cout << endl
         << "ERROR <xtp_map> : state file '" << _outdb
         << "' already in use. Abort." << endl;
    _statsav.Close();
    return;
  }

  string cgfile = _op_vm["segments"].as<string>();
  _md2qm.Initialize(cgfile);

  // ++++++++++++++++++++++++++++ //
  // Create MD topology from file //
  // ++++++++++++++++++++++++++++ //

  // Create topology reader
  string topfile = _op_vm["topology"].as<string>();
  CSG::TopologyReader *topread;
  std::unique_ptr<CSG::TopologyReader> topread =
      std::unique_ptr<CSG::TopologyReader>(
          CSG::TopReaderFactory().Create(topfile));

  if (topread == nullptr) {
    throw runtime_error(string("Input format not supported: ") +
                        _op_vm["topology"].as<string>());
  }

  topread->ReadTopology(topfile, this->_mdtopol);
  if (TOOLS::globals::verbose) {
    cout << "Read MD topology from " << topfile << ": Found "
         << _mdtopol.BeadCount() << " atoms in " << _mdtopol.MoleculeCount()
         << " molecules. " << endl;
  }

  // ++++++++++++++++++++++++++++++ //
  // Create MD trajectory from file //
  // ++++++++++++++++++++++++++++++ //

  // Create trajectory reader and initialize
  string trjfile = _op_vm["coordinates"].as<string>();
  std::unique_ptr<CSG::TrajectoryReader> trjread =
      std::unique_ptr<CSG::TrajectoryReader>(
          CSG::TrjReaderFactory().Create(trjfile));

  if (trjread == nullptr) {
    throw runtime_error(string("Input format not supported: ") +
                        _op_vm["coordinates"].as<string>());
  }
  trjread->Open(trjfile);
  trjread->FirstFrame(_mdtopol);

  int firstFrame = 1;
  int nFrames = 1;
  bool beginAt = 0;
  double startTime = _mdtopol.getTime();

  if (_op_vm.count("nframes")) {
    nFrames = _op_vm["nframes"].as<int>();
  }
  if (_op_vm.count("first-frame")) {
    firstFrame = _op_vm["first-frame"].as<int>();
  }
  if (_op_vm.count("begin")) {
    beginAt = true;
    startTime = _op_vm["begin"].as<double>();
  }

  // Extract first frame specified
  bool hasFrame;

  for (hasFrame = true; hasFrame == true;
       hasFrame = trjread->NextFrame(_mdtopol)) {
    if (((_mdtopol.getTime() < startTime) && beginAt) || firstFrame > 1) {
      firstFrame--;
      continue;
    }
    break;
  }
  if (!hasFrame) {
    trjread->Close();

    throw runtime_error("Time or frame number exceeds trajectory length");
  }

  // +++++++++++++++++++++++++ //
  // Convert MD to QM Topology //
  // +++++++++++++++++++++++++ //

  for (int saved = 0; hasFrame && saved < nFrames;
       hasFrame = trjread->NextFrame(_mdtopol), saved++) {
    _md2qm.Md2Qm(&_mdtopol, &_qmtopol);
    Save();
  }
}

void XtpMap::Save() {

  _statsav.Open(_outdb);
  _statsav.WriteFrame(_qmtopol);
  _statsav.Close();
}

void XtpMap::ShowHelpText(std::ostream &out) {
  string name = ProgramName();
  if (VersionString() != "") name = name + ", version " + VersionString();
  XTP::HelpTextHeader(name);
  HelpText(out);
  out << "\n\n" << VisibleOptions() << endl;
}

int main(int argc, char **argv) {
  XtpMap xtpmap;
  return xtpmap.Exec(argc, argv);
}
