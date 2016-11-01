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

#ifndef VOTCA_XTP_POLARBACKGROUND_H
#define VOTCA_XTP_POLARBACKGROUND_H


#include <votca/xtp/ewaldactor.h>
#include <votca/xtp/xinteractor.h>
#include <votca/xtp/logger.h>
#include <votca/xtp/threadforce.h>
#include <votca/tools/globals.h>


namespace votca { namespace ctp {    
namespace EWD {

class PolarBackground
{
public:

    PolarBackground() : _log(NULL),
            _n_threads(1),
            _top(NULL),
            _ptop(NULL) {};
    PolarBackground(Topology *, PolarTop *, Property *, Logger *);
   ~PolarBackground();
   
    void Threaded(int n_threads) { _n_threads = n_threads; }
    void Polarize(int n_threads);
    void Checkpoint(int iter, bool converged);
    bool HasConverged() { return _converged; }
    
    void FX_RealSpace(std::string mode, bool do_setup_nbs);    
    void FX_ReciprocalSpace(std::string S_mode, std::string F_mode, bool gen_kvecs);    
    void GenerateKVectors(std::vector<PolarSeg*> &ps1, std::vector<PolarSeg*> &ps2);
    
    // TODO Would be nicer to have RThread / KThread as a local type in ...
    //      ... but you cannot use local types as template parameters
    //      (which is however possible by compiling with --std=c++0x)
    
    class RThread :
        public MultiModeThread
            <votca::tools::Thread, EWD::PolarBackground::RThread>
    {
    public:        
        RThread(PolarBackground *master, bool do_setup_nbs) {
            _master = master;
            _do_setup_nbs = do_setup_nbs;
            _not_converged_count = 0;
            _ewdactor = EwdInteractor(_master->_alpha, _master->_polar_aDamp);
            _actor = XInteractor(NULL, _master->_polar_aDamp);
            
            RegisterStart("FP_MODE", &RThread::FP_FieldCalc);
            RegisterStart("FU_MODE", &RThread::FU_FieldCalc);            
            RegisterReset("FP_MODE", &RThread::FX_FieldReset);
            RegisterReset("FU_MODE", &RThread::FX_FieldReset);            
            RegisterWload("FP_MODE", &RThread::FX_FieldWload);
            RegisterWload("FU_MODE", &RThread::FX_FieldWload);
        }
       ~RThread() {
           _full_bg_P.clear(); _part_bg_P.clear();
        }
       
        RThread *Clone() { return new RThread(_master, _do_setup_nbs); }
        
        // Input
        void AddSharedInput(std::vector<PolarSeg*> &fullbg) { _full_bg_P = fullbg; }
        void AddAtomicInput(PolarSeg *pseg) { _part_bg_P.push_back(pseg); }        
        // Mode targets
        void FU_FieldCalc();
        void FP_FieldCalc();
        void FX_FieldReset() { _not_converged_count = 0; _part_bg_P.clear(); }
        double FX_FieldWload() { return 100.*_part_bg_P.size()/_full_bg_P.size(); }        
        // Convergence & output related
        const int &NotConverged() { return _not_converged_count; }
        double AvgRco() { return _avg_R_co; }
        
    private:
        
        PolarBackground *_master;
        EwdInteractor _ewdactor;      // Long-range tensors (default)
        XInteractor _actor;           // Cut-off tensors (option)
        // Shared thread data        
        std::vector<PolarSeg*> _full_bg_P;        
        // Atomized thread data
        std::vector<PolarSeg*> _part_bg_P;        
        // Convergence & output-related
        int _not_converged_count;
        double _avg_R_co;
        bool _do_setup_nbs;
    };
    
    class KThread : 
        public MultiModeThread
            <votca::tools::Thread, EWD::PolarBackground::KThread>
    {
    public:
        
        KThread(PolarBackground *master) {
            _master = master;
            _ewdactor = EwdInteractor(_master->_alpha, _master->_polar_aDamp);
           
            RegisterStart("SP_MODE", &KThread::SP_SFactorCalc);
            RegisterStart("FP_MODE", &KThread::FP_KFieldCalc);
            RegisterStart("SU_MODE", &KThread::SU_SFactorCalc);
            RegisterStart("FU_MODE", &KThread::FU_KFieldCalc);            
            RegisterReset("SP_MODE", &KThread::SFactorReset);
            RegisterReset("FP_MODE", &KThread::KFieldReset);
            RegisterReset("SU_MODE", &KThread::SFactorReset);
            RegisterReset("FU_MODE", &KThread::KFieldReset);            
            RegisterWload("SP_MODE", &KThread::SFactorWload);
            RegisterWload("FP_MODE", &KThread::KFieldWload);
            RegisterWload("SU_MODE", &KThread::SFactorWload);
            RegisterWload("FU_MODE", &KThread::KFieldWload);
        }
       ~KThread() {
           _full_bg_P.clear(); _full_kvecs.clear(); 
           _part_bg_P.clear(); _part_kvecs.clear();
        }
        
        KThread *Clone() { return new KThread(_master); }
        
        void AddSharedInput(std::vector<EWD::KVector*> &full_kvecs) 
            { _full_kvecs.clear(); _full_kvecs = full_kvecs; }
        void AddSharedInput(std::vector<PolarSeg*> &full_bg_P) 
            { _full_bg_P.clear(); _full_bg_P = full_bg_P; }
        
        // MODE 1 : Compute structure factor of _part_kvecs using _full_bg_P
        void SP_SFactorCalc();
        void SU_SFactorCalc();
        void SFactorReset() { _part_kvecs.clear(); _full_kvecs.clear(); }
        void AddAtomicInput(EWD::KVector *k) { _part_kvecs.push_back(k); }        
        double SFactorWload() { return 1.*_part_kvecs.size()/_full_kvecs.size(); }
        
        // MODE 2 : Increment fields of _part_bg_P using _full_kvecs
        void FP_KFieldCalc();
        void FU_KFieldCalc();
        void KFieldReset() { ; }
        void AddAtomicInput(PolarSeg *pseg) { _part_bg_P.push_back(pseg); }        
        double KFieldWload() { return 1.*_part_bg_P.size()/_full_bg_P.size(); }
        
    private:
        PolarBackground *_master;
        EwdInteractor _ewdactor;
        // Shared thread data        
        std::vector<PolarSeg*> _full_bg_P;
        std::vector<EWD::KVector*> _full_kvecs;        
        // Atomized thread data        
        std::vector<EWD::KVector*> _part_kvecs;
        std::vector<PolarSeg*> _part_bg_P;
    
    public:
        // Convergence info
        double _rms_sum_re;
        double _sum_im;
    };
    
    
private:

    EwdInteractor _ewdactor;                  // Long-range scheme (default)
    XInteractor _actor;                       // Cut-off scheme (option)
    Logger *_log;
    int _n_threads;

    // CHECKPOINTING & RESTART OPTIONS
    bool _do_checkpointing;
    bool _do_restart;
    int _restart_from_iter;
    int _max_iter;
    bool _converged;
    
    // PERIODIC BOUNDARY
    Topology *_top;
    std::string _shape;
    bool _do_use_cutoff;

    // POLAR SEGMENTS
    // Part I - Ewald
    PolarTop *_ptop;
    std::vector< PolarSeg* > _bg_P;               // Period. density = _bg_N v _fg_N
    bool _do_compensate_net_dipole;          
    std::string _dipole_compensation_type;        // "system" or "segment"
    std::string _dipole_compensation_direction;   // "xyz" or "z"
    
    // CONVERGENCE
    // Part I - Ewald
    double _alpha;                  // _a = 1/(sqrt(2)*sigma)
    double _kfactor;
    double _rfactor;
    double _K_co;                   // k-space c/o
    double _R_co;                   // r-space c/o
    double _crit_dE;                // Energy convergence criterion [eV]
    //bool   _converged_R;            // Did R-space sum converge?
    //bool   _converged_K;            // Did K-space sum converge?
    //bool   _field_converged_R;
    bool   _field_converged_K;
    // Part II - Thole
    double _polar_aDamp;
    double _polar_wSOR_N;
    double _polar_cutoff;

    // LATTICE (REAL, RECIPROCAL)
    vec _a; vec _b; vec _c;         // Real-space lattice vectors
    int _na_max, _nb_max, _nc_max;  // Max. cell indices to sum over (R)
    vec _A; vec _B; vec _C;         // Reciprocal-space lattice vectors
    int _NA_max, _NB_max, _NC_max;  // Max. cell indices to sum over (K)
    double _LxLy;                   // |a^b|
    double _LxLyLz;                 // a*|b^c|

    EWD::VectorSort<EWD::KNorm,EWD::KVector> _kvecsort;
    std::vector<EWD::KVector*> _kvecs_2_0;   // K-vectors with two components = zero
    std::vector<EWD::KVector*> _kvecs_1_0;   // K-vectors with one component  = zero
    std::vector<EWD::KVector*> _kvecs_0_0;   // K-vectors with no  components = zero
    double _kxyz_s1s2_norm;

};    

}}}

#endif
