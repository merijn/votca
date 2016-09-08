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

#ifndef _VOTCA_XTP_QMANALYZE_H
#define _VOTCA_XTP_QMANALYZE_H

#include <stdio.h>
#include <votca/tools/constants.h>
#include <votca/xtp/logger.h>
// #include <votca/xtp/mbgft.h>
// #include <votca/xtp/qmpackagefactory.h>

namespace votca { namespace xtp {
    using namespace std;
    namespace ub = boost::numeric::ublas;
class QMAnalyze : public QMTool
{
public:

    QMAnalyze() { };
   ~QMAnalyze() { };

    string Identify() { return "qmanalyze"; }

    void   Initialize(Property *options);
    bool   Evaluate();


private:
    
    string      _orbfile;
 
    bool _print_BSE_singlets;
    bool _print_BSE_triplets;
    bool _print_GW_energies;
    bool _print_QP_energies;
    bool _print_DFT_energies;
    
    
    Logger      _log;
    
    void CheckContent(  Orbitals& _orbitals );

};

void QMAnalyze::Initialize(Property* options) {
            
    
    _print_BSE_singlets=false;
    _print_BSE_triplets=false;
    _print_GW_energies=false;
    _print_QP_energies=false;
    _print_DFT_energies=false;
            // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options, "xtp" );
 

    string key = "options." + Identify();
    // _jobfile = options->get(key + ".file").as<string>();

    // key = "options." + Identify();


   // orbitals file or pure DFT output
   _orbfile      = options->get(key + ".input").as<string> ();


   if ( options->exists(key+".output")) {
       string _store_string= options->get(key + ".output").as<string> ();
        
        if (_store_string.find("singlets") != std::string::npos) _print_BSE_singlets=true;
        if (_store_string.find("triplets") != std::string::npos) _print_BSE_triplets=true;
        
        if (_store_string.find("DFT") != std::string::npos) _print_DFT_energies=true;
        if (_store_string.find("GW") != std::string::npos) _print_GW_energies=true;
        if (_store_string.find("QP") != std::string::npos) _print_QP_energies=true;
    
   
  
        
    
        
   }
   
   
    
  
    // get the path to the shared folders with xml files
    char *votca_share = getenv("VOTCASHARE");    
    if(votca_share == NULL) throw std::runtime_error("VOTCASHARE not set, cannot open help files.");
    // string xmlFile = string(getenv("VOTCASHARE")) + string("/xtp/packages/") + _package + string("_idft_pair.xml");
    // load_property_from_xml( _package_options, xmlFile );    

    // register all QM packages (Gaussian, TURBOMOLE, etc)
    // QMPackageFactory::RegisterAll();
    
    
    
    
    
}

bool QMAnalyze::Evaluate() {

    _log.setReportLevel( logDEBUG );
    _log.setMultithreading( true );
    
    _log.setPreface(logINFO,    "\n... ...");
    _log.setPreface(logERROR,   "\n... ...");
    _log.setPreface(logWARNING, "\n... ...");
    _log.setPreface(logDEBUG,   "\n... ..."); 

    LOG(logDEBUG, _log) << "Analyzing serialized QM data in " << _orbfile << flush;

    Orbitals _orbitals;
    // load the QM data from serialized orbitals object

    std::ifstream ifs( (_orbfile).c_str());
    LOG(logDEBUG, _log) << " Loading QM data from " << _orbfile << flush;
    boost::archive::binary_iarchive ia(ifs);
    ia >> _orbitals;
    ifs.close();
    
    // output info about contents of serialized data
    CheckContent( _orbitals );
    
    
    
    
  
    
    
    return true;
}




void QMAnalyze::CheckContent( Orbitals& _orbitals ){


   
    LOG(logDEBUG, _log) << "===== Summary of serialized content ===== " << flush;
    LOG(logDEBUG, _log) << "   Information about DFT:" << flush;
    
          
    // DFT atoms
    if ( _orbitals.hasQMAtoms() ) {
        LOG(logDEBUG, _log) << "      atoms:                  " << _orbitals.QMAtoms().size() << flush;
    } else {
        LOG(logDEBUG, _log) << "      atoms:                  not stored "<< flush;
    } 
    
    // QM package
    if ( _orbitals.hasQMpackage() ){
        LOG(logDEBUG, _log) << "      QM package:             " << _orbitals.getQMpackage() << flush;
    
    } else {
        LOG(logDEBUG, _log) << "      QM package:             not stored " << flush;
    }
    
    
    // DFT basis set
    if ( _orbitals.hasDFTbasis() ) {
        LOG(logDEBUG, _log) << "      basis set:              " << _orbitals.getDFTbasis() << flush;
    } else {
        LOG(logDEBUG, _log) << "      basis set:              not stored "<< flush;
    }

    // DFT basis set size
    if ( _orbitals.hasBasisSetSize() ) {
        LOG(logDEBUG, _log) << "      basis set size:         " << _orbitals.getBasisSetSize() << flush;
   
    } else {
        LOG(logDEBUG, _log) << "      basis set size:          not stored "<< flush;
    }

    // DFT number of electrons
    if ( _orbitals.hasNumberOfElectrons() ) {
        LOG(logDEBUG, _log) << "      number of electrons:    " << _orbitals.getNumberOfElectrons() << flush;
    } else {
         LOG(logDEBUG, _log) << "      number of electrons:    not stored "<< flush;
    }    
    
    // DFT number of levels
    if ( _orbitals.hasNumberOfLevels() ) {
        LOG(logDEBUG, _log) << "      number of levels:       " << _orbitals.getNumberOfLevels() << flush;
    } else {
         LOG(logDEBUG, _log) << "      number of levels:       not stored "<< flush;
    }    

    // DFT orbital energies
    if ( _orbitals.hasMOEnergies() ) {
        LOG(logDEBUG, _log) << "      MO energies:            " << _orbitals.getEnergies()->size() << flush;
    } else {
         LOG(logDEBUG, _log) << "      MO energies:            not stored "<< flush;
    }    

    // DFT orbital coefficients
    if ( _orbitals.hasMOCoefficients() ) {
        LOG(logDEBUG, _log) << "      MO coefficients:        " << _orbitals.MOCoefficients().size1() << " x " << _orbitals.MOCoefficients().size2() << flush;
    } else {
        LOG(logDEBUG, _log) << "      MO coefficients:        not stored "<< flush;
    }  
    
    // DFT AO overlap matrix
    if ( _orbitals.hasAOOverlap() ) {
        LOG(logDEBUG, _log) << "      AO overlap matrix:      " << _orbitals.getOverlap()->size1()  << " x " << _orbitals.getOverlap()->size2() << flush;
    } else {
        LOG(logDEBUG, _log) << "      AO overlap matrix:      not stored "<< flush;
    }    
    
    // DFT AO XC matrix
    if ( _orbitals.hasAOVxc() ) {
        LOG(logDEBUG, _log) << "      AO XC matrix:           " << _orbitals.AOVxc().size1()  << " x " << _orbitals.AOVxc().size2() << flush;
    } else {
        LOG(logDEBUG, _log) << "      AO XC matrix:           not stored "<< flush;
    }    

    // QM total energy
    if ( _orbitals.hasQMEnergy() ){
        LOG(logDEBUG, _log) << "      QM energy[eV]:          " << _orbitals.getQMEnergy() << flush;
    } else{
        LOG(logDEBUG, _log) << "      QM energy:              not stored " << flush;
    }
    
    // MM self-energy 
    if ( _orbitals.hasSelfEnergy() ){
        LOG(logDEBUG, _log) << "      MM self energy[eV]:     " << _orbitals.getSelfEnergy() << flush;
    } else{
        LOG(logDEBUG, _log) << "      MM self energy:         not stored " << flush;
    }    
    
    // DFT transfer integrals
    if ( _orbitals.hasMOCouplings() ) {
        LOG(logDEBUG, _log) << "      DFT transfer integrals: " << _orbitals.MOCouplings().size1() << " x " << _orbitals.MOCouplings().size2() << flush;

    } else {
         LOG(logDEBUG, _log) << "      DFT transfer integrals: not stored "<< flush;
    }    
    
    
    

    LOG(logDEBUG, _log) << "   Information about GWA:" << flush;
    
    // GWA basis set
    if ( _orbitals.hasGWbasis() ) {
        LOG(logDEBUG, _log) << "      basis set:              " << _orbitals.getGWbasis() << flush;
    } else {
        LOG(logDEBUG, _log) << "      basis set:              not stored "<< flush;
    }
    
    // RPA index range
    if ( _orbitals.hasRPAindices() ){
        LOG(logDEBUG, _log) << "      RPA level range:        " << _orbitals.getRPAmin() << " : " << _orbitals.getRPAmax() << flush;        
    } else {
        LOG(logDEBUG, _log) << "      RPA level range:        not stored" << flush;
    }

    // GWA index range
    if ( _orbitals.hasGWAindices() ){
        LOG(logDEBUG, _log) << "      GWA level range:        " << _orbitals.getGWAmin() << " : " << _orbitals.getGWAmax() << flush;        
    } else {
        LOG(logDEBUG, _log) << "      GWA level range:        not stored" << flush;
    }
    
    // perturbative QP energies
    if ( _orbitals.hasQPpert()){
        LOG(logDEBUG, _log) << "      number of QP levels:    " << _orbitals.QPpertEnergies().size1() << flush;
        if (_print_GW_energies){
            int _qpmin=_orbitals.getGWAmin();
            int _homo=_orbitals.getNumberOfElectrons()-1;
         
            const ub::matrix<double>& _qp_energies=_orbitals.QPpertEnergies();
           
            double _shift= _qp_energies( _homo+1-_qpmin,4) - _qp_energies( _homo-_qpmin,4 )-_qp_energies( _homo+1-_qpmin,0) + _qp_energies( _homo-_qpmin,0 ); 
            
            LOG(logDEBUG,_log) << (format("  ====== Perturbative quasiparticle energies (Rydberg) ====== ")).str() << flush;
            LOG(logDEBUG,_log) << (format("   DeltaHLGap = %1$+1.6f Ryd") % _shift ).str()  <<  flush;
            for ( int _i = 0 ; _i < _orbitals.getGWAtot() ; _i++ ){
                if ( (_i + _qpmin) == _homo ){
                    LOG(logINFO,_log) << (format("  HOMO  = %1$4d DFT = %2$+1.4f VXC = %3$+1.4f S-X = %4$+1.4f S-C = %5$+1.4f GWA = %6$+1.4f") % (_i+_qpmin+1) % _qp_energies( _i, 0 ) %_qp_energies( _i, 1 ) % _qp_energies( _i, 2 ) %_qp_energies( _i, 3 ) % _qp_energies( _i, 4 ) ).str() << flush;
                } else if ( (_i + _qpmin) == _homo+1 ){
                    LOG(logINFO,_log) << (format("  LUMO  = %1$4d DFT = %2$+1.4f VXC = %3$+1.4f S-X = %4$+1.4f S-C = %5$+1.4f GWA = %6$+1.4f") % (_i+_qpmin+1) % _qp_energies( _i, 0 ) % _qp_energies( _i, 1 ) % _qp_energies( _i, 2 ) % _qp_energies( _i, 3 ) % _qp_energies( _i, 4 ) ).str() << flush;                    
                    
                }else {
                LOG(logINFO,_log) << (format("  Level = %1$4d DFT = %2$+1.4f VXC = %3$+1.4f S-X = %4$+1.4f S-C = %5$+1.4f GWA = %6$+1.4f") % (_i+_qpmin+1) % _qp_energies( _i, 0 ) % _qp_energies( _i, 1 ) % _qp_energies( _i, 2 ) %_qp_energies( _i, 3) % _qp_energies( _i, 4 ) ).str() << flush;
                }
            }
        }
    } else {
        LOG(logDEBUG, _log) << "      number of QP levels:    not stored" << flush;
    }
    
    // diagonalized QP energies
    if ( _orbitals.hasQPdiag() ){
        LOG(logDEBUG, _log) << "      diagonalized QP levels: " << _orbitals.QPdiagEnergies().size() << flush;
        if(_print_QP_energies){
            unsigned _qpmin=_orbitals.getGWAmin();
            unsigned _homo=_orbitals.getNumberOfElectrons()-1;
            const ub::vector<double>& _qp_diag_energies=_orbitals.QPdiagEnergies();
             const ub::matrix<double>& _qp_energies=_orbitals.QPpertEnergies();
                    LOG(logDEBUG, _log) << TimeStamp() << " Full quasiparticle Hamiltonian  " << flush;
                    LOG(logDEBUG, _log) << (format("  ====== Diagonalized quasiparticle energies (Rydberg) ====== ")).str() << flush;
                    for (unsigned _i = 0; _i <  _qp_diag_energies.size(); _i++) {
                        if (( _qpmin+ _i) == _homo) {
                            LOG(logDEBUG, _log) << (format("  HOMO  = %1$4d PQP = %2$+1.4f DQP = %3$+1.4f ") % (_i + _qpmin + 1) % _qp_energies(_i + _qpmin,4 ) % _qp_diag_energies(_i)).str() << flush;
                        } else if ((_qpmin+ _i) == _homo + 1) {
                            LOG(logDEBUG, _log) << (format("  LUMO  = %1$4d PQP = %2$+1.4f DQP = %3$+1.4f ") % (_i + _qpmin + 1) % _qp_energies(_i + _qpmin,4) % _qp_diag_energies(_i)).str() << flush;

                        } else {
                            LOG(logDEBUG, _log) << (format("  Level = %1$4d PQP = %2$+1.4f DQP = %3$+1.4f ") % (_i + _qpmin + 1) % _qp_energies(_i + _qpmin,4 ) % _qp_diag_energies(_i)).str() << flush;
                        }
                    }
        }
        
    } else {
        LOG(logDEBUG, _log) << "      diagonalized QP levels: not stored" << flush;
    }


    LOG(logDEBUG, _log) << "   Information about BSE:" << flush;

    // BSE index range
    if ( _orbitals.hasBSEindices() ){
        LOG(logDEBUG, _log) << "      BSE level range:        [" << _orbitals.getBSEvmin() << " : " << _orbitals.getBSEvmax()  << "] x [ " << _orbitals.getBSEcmin() << " : " << _orbitals.getBSEcmax() << "]" << flush;        
    } else {
        LOG(logDEBUG, _log) << "      BSE level range:        not stored" << flush;
    }
    
    // BSE EH interaction
    if ( _orbitals.hasEHinteraction() ){
        LOG(logDEBUG, _log) << "      direct interaction:     " << _orbitals.eh_d().size1() << flush;
        LOG(logDEBUG, _log) << "      exchange interaction:   " << _orbitals.eh_x().size1() << flush;
    } else {
        LOG(logDEBUG, _log) << "      e-h interactions:       not stored" << flush;
    }
    
    // BSE singlet excitons
    if ( _orbitals.hasBSESinglets()){
        LOG(logDEBUG, _log) << "      BSE singlet excitons:   " << _orbitals.getBSESingletEnergies()->size() << flush;
        
        if (_print_BSE_singlets){
            LOG(logINFO, _log) << (format("  ====== singlet energies (eV) ====== ")).str() << flush;
            const ub::vector<real_gwbse> &  _bse_singlet_energies = _orbitals.BSESingletEnergies();
            const std::vector<ub::vector<double> > & _transition_dipoles=_orbitals.TransitionDipoles();
            unsigned size=_bse_singlet_energies.size();
            if (_transition_dipoles.size()<_bse_singlet_energies.size()){
                size=_transition_dipoles.size();
            }
            for (unsigned _i=0;_i<size;_i++){
                
                LOG(logINFO, _log) << (format("  S = %1$4d Omega = %2$+1.12f eV  lamdba = %3$+3.2f nm")
                    % (_i + 1) % (tools::conv::ryd2ev * _bse_singlet_energies(_i)) % (1240.0/(tools::conv::ryd2ev * _bse_singlet_energies(_i)))).str() << flush;
                if ( _orbitals.hasTransitionDipoles()){
                    double trstrength =ub::inner_prod(_transition_dipoles[_i],_transition_dipoles[_i]);
                    
                    double oscstrength =trstrength/3.0*_bse_singlet_energies[_i];
                    LOG(logINFO, _log) << (format("           TrDipole length gauge[e*bohr]  dx = %1$+1.4f dy = %2$+1.4f dz = %3$+1.4f |d|^2 = %4$+1.4f f = %5$+1.4f") 
                                    % (_transition_dipoles[_i](0)) % (_transition_dipoles[_i](1)) % (_transition_dipoles[_i](2)) % (trstrength) 
                                    % oscstrength).str() << flush;
                }
                
                
                
            }
        }    
        
            
        
        
        
        
    } else {
        LOG(logDEBUG, _log) << "      BSE singlet excitons:   not stored" << flush;
    }  
    
    
    
    // Transition dipole moments
    if ( !_orbitals.hasTransitionDipoles()){
        LOG(logDEBUG, _log) << "      BSE transition dipoles: not stored" << flush;
    }  
    
       
    
    // BSE triplet excitons
    if ( _orbitals.hasBSETriplets()){
        LOG(logDEBUG, _log) << "      BSE triplet excitons:   " << _orbitals.getBSETripletEnergies()->size() << flush;
        
        
        if(_print_BSE_triplets){
             LOG(logINFO, _log) << (format("  ====== triplet energies (eV) ====== ")).str() << flush;
             const ub::vector<real_gwbse> &  _bse_triplet_energies = _orbitals.BSETripletEnergies();
             cout << _bse_triplet_energies.size()<<endl;
             for (unsigned _i=0;_i<_bse_triplet_energies.size();_i++){
             LOG(logINFO, _log) << (format("  T = %1$4d Omega = %2$+1.12f eV  lamdba = %3$+3.2f nm")
                                % (_i + 1) % (tools::conv::ryd2ev * _bse_triplet_energies(_i)) % (1240.0/(13.6058 * _bse_triplet_energies(_i)))).str() << flush;
             
            }
        }
    } else {
        LOG(logDEBUG, _log) << "      BSE triplet excitons:   not stored" << flush;
    }  


    // BSE singlet couplings
    if ( _orbitals.hasSingletCouplings()){
        LOG(logDEBUG, _log) << "      BSE singlet couplings:  between " << _orbitals.getCoupledExcitonsA() << " lowest excitons" << flush;
    } else {
        LOG(logDEBUG, _log) << "      BSE singlet couplings:  not stored" << flush;
    } 

    
    // BSE triplet couplings
    if ( _orbitals.hasTripletCouplings()){
        LOG(logDEBUG, _log) << "      BSE triplet couplings:  between " << _orbitals.getCoupledExcitonsA() << " lowest excitons" << flush;
    } else {
        LOG(logDEBUG, _log) << "      BSE triplet couplings:  not stored" << flush;
    }  
    
   
    
    
    
}

}}


#endif
