/* 
 *            Copyright 2009-2016 The VOTCA Development Team
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

#ifndef VOTCA_XTP_INTEGRALSEXTRACTOR_H
#define VOTCA_XTP_INTEGRALSEXTRACTOR_H

#include <votca/tools/propertyiomanipulator.h>
#include <votca/xtp/qmcalculator.h>
#include <boost/format.hpp>

namespace votca { namespace ctp {


class IntegralsExtractor : public XQMCalculator
{
public:

    IntegralsExtractor() { };
   ~IntegralsExtractor() { };

    string Identify() { return "extract.integrals"; }
    void Initialize(Property *options);
    bool EvaluateFrame(Topology *top);

private:

};


void IntegralsExtractor::Initialize(Property *options) {
    return;
}


bool IntegralsExtractor::EvaluateFrame(Topology *top) {
    
    string xmlfile = Identify() + ".xml";    
    
    Property state("state", "", "");
    Property &pairs = state.add("pairs","");    
    
    using boost::format;
    
    // PAIRS
    QMNBList::iterator pit;
    QMNBList &nb = top->NBList();
    for (pit = nb.begin(); pit != nb.end(); ++pit) {
        QMPair *qmp = *pit;
        Property &pairprop = pairs.add("pair", "");
        pairprop.add("id1",   (format("%1$d")   % qmp->Seg1()->getId()).str());
        pairprop.add("name1", (format("%1$s")   % qmp->Seg1()->getName()).str());
        pairprop.add("id2",   (format("%1$d")   % qmp->Seg2()->getId()).str());
        pairprop.add("name2", (format("%1$s")   % qmp->Seg2()->getName()).str());

        if (qmp->isPathCarrier(+1)) {
            Property &channel = pairprop.add("channel", "");
            channel.setAttribute("type","hole");
            channel.add("jeff2_h", (format("%1$1.7e") % qmp->getJeff2(+1)).str());
        }
        if (qmp->isPathCarrier(-1)) {
            Property &channel = pairprop.add("channel", "");
            channel.setAttribute("type","electron");
            channel.add("jeff2_e", (format("%1$1.7e") % qmp->getJeff2(-1)).str());
        }
    }
    
    ofstream ofs;    
    ofs.open(xmlfile.c_str(), ofstream::out);
    if (!ofs.is_open()) {
        throw runtime_error("Bad file handle: " + xmlfile);
    }
    ofs << tools::XML << state;
    ofs.close();
    
    return true;
}


}}

#endif // VOTCA_XTP_INTEGRALSEXTRACTOR_H
