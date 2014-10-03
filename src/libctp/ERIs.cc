/* 
 *            Copyright 2009-2012 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICEN_olE-2.0
 *
 *Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <votca/ctp/aomatrix.h>
#include <votca/ctp/aobasis.h>
#include <votca/ctp/segment.h>
#include <votca/ctp/threecenters.h>
#include <string>
#include <map>
#include <vector>
#include <votca/tools/property.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/multi_array.hpp>
#include <omp.h>
#include <votca/ctp/ERIs.h>

using namespace std;
using namespace votca::tools;

namespace votca {
    namespace ctp {
        namespace ub = boost::numeric::ublas;
        
        ERIs::~ERIs () { ; }
        
        ERIs::ERIs( AOBasis &dftbasis, AOBasis &auxbasis)
                :ERIs( &dftbasis, &auxbasis ) {}
        
        
        void Initialize (){
        
            AOOverlap Overlap;
            
        
        
        
        
        
        
        
        }
        
        
        
        
        
        
    }}