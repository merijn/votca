/*
 * Copyright 2009-2017 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VOTCA_CSG_PDBTRAJECTORYREADER_H
#define	__VOTCA_CSG_PDBTRAJECTORYREADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <votca/csg/trajectoryreader.h>

namespace votca { namespace csg {
using namespace votca::tools;

using namespace std;

/**
    \brief class for reading pdb files

    This class provides the TrajectoryReader reader interface
    for pdb files

*/
class PDBTrajectoryReader : public TrajectoryReader
{
    public:
        
        /// open a trejectory file
        bool Open(const string &file);
        /// read in the first frame
        bool FirstFrame(Topology &top);
        /// read in the next frame
        bool NextFrame(Topology &top);
        /// close original trajectory file
        void Close();

        /// set/get the original configuration or trajectory file name:
        /// <name>.pdb/<name>.pdb (convention: ".pdb"="CONFIG", ".pdb"="HISTORY")
        void setFname(string name) { _fname = name; return; }
        string getFname()          { return _fname; }

        /// set/check the flag for the read-in file as configuration, i.e. not trajectory format
        void setIsConfig(bool isConf) { _isConfig=isConf; return; }
        void getIsConfig()            { _isConfig; }

    private:
        ifstream _fl;
        string _fname;
        bool _first_frame;
        bool _isConfig;
};

}}

#endif // __VOTCA_CSG_PDBTRAJECTORYREADER_H
