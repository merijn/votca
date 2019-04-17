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

#ifndef VOTCA_XTP_JOBCALCULATOR_H
#define VOTCA_XTP_JOBCALCULATOR_H

#include <votca/xtp/job.h>
#include <votca/xtp/progressobserver.h>
#include <votca/xtp/qmcalculator.h>
#include <votca/xtp/topology.h>

namespace votca {
namespace xtp {

class Topology;

class JobCalculator : public QMCalculator {
 public:
  JobCalculator() {}
  virtual ~JobCalculator() {}

  virtual std::string Identify() = 0;

  virtual bool EvaluateFrame(Topology &top) = 0;

  virtual void WriteJobFile(Topology &top) = 0;
  virtual void ReadJobFile(Topology &top) = 0;

  void setProgObserver(ProgObserver<std::vector<Job> > *obs) { _progObs = obs; }

 protected:
  ProgObserver<std::vector<Job> > *_progObs;
};

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_JOBCALCULATOR_H
