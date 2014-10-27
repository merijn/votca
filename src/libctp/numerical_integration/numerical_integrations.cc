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
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "A_ol I_ol" BA_olI_ol,
 * WITHOUT WARRANTIE_ol OR CONDITION_ol OF ANY KIND, either express or implied.
 * _olee the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
// Overload of uBLAS prod function with MKL/GSL implementations
#include <votca/ctp/votca_ctp_config.h>


#include <votca/ctp/numerical_integrations.h>
#include <boost/math/constants/constants.hpp>
#include <votca/ctp/radial_euler_maclaurin_rule.h>
#include <votca/ctp/sphere_lebedev_rule.h>
#include <votca/ctp/exchange_correlation.h>
#include <fstream>
// #include <xc.h>
using namespace std;


namespace votca {
    namespace ctp {
        namespace ub = boost::numeric::ublas;

        
        
        
        // numerically integrate the elements of the AOXC matrix
        ub::matrix<double> NumericalIntegration::IntegrateVXC(ub::matrix<double>& _density_matrix, AOBasis* basis){
            
            
            xc_func_type xfunc; // handle for exchange functional
            xc_func_type cfunc; // handle for correleation functions

            // define PBE here (should be some optional setting))
            int xfunc_id = XC_GGA_X_PBE;
            int cfunc_id = XC_GGA_C_PBE;
            if(xc_func_init(&xfunc, xfunc_id, XC_UNPOLARIZED) != 0){
               fprintf(stderr, "Functional '%d' not found\n", xfunc_id);
               exit(1);
            }
            
            if(xc_func_init(&cfunc, cfunc_id, XC_UNPOLARIZED) != 0){
               fprintf(stderr, "Functional '%d' not found\n", cfunc_id);
               exit(1);
            }
            
            printf("The exchange functional '%s' is defined in the reference(s):\n%s\n", xfunc.info->name, xfunc.info->refs);
            printf("The correlation functional '%s' is defined in the reference(s):\n%s\n", cfunc.info->name, cfunc.info->refs);

            // xc_func_end(&xfunc);
            
            ExchangeCorrelation _xc;
            ub::matrix<double> XCMAT = ub::zero_matrix<double>(basis->_AOBasisSize, basis->_AOBasisSize);
            const ub::vector<double> DMAT_array=_density_matrix.data();
            // for every gridpoint
            for (int i = 0; i < _grid.size(); i++) {
                for (int j = 0; j < _grid[i].size(); j++) {
                    // get value of orbitals at each gridpoint
                    ub::matrix<double> AOatgrid = ub::zero_matrix<double>(basis->_AOBasisSize, 1);
                    
                    ub::matrix<double> AODerXatgrid = ub::zero_matrix<double>(basis->_AOBasisSize, 1); // for Gradients of AOs
                    ub::matrix<double> AODerYatgrid = ub::zero_matrix<double>(basis->_AOBasisSize, 1); // for Gradients of AOs
                    ub::matrix<double> AODerZatgrid = ub::zero_matrix<double>(basis->_AOBasisSize, 1); // for Gradients of AOs
                    
                    for (vector< AOShell* >::iterator _row = basis->firstShell(); _row != basis->lastShell(); _row++) {

                        // for density
                        ub::matrix_range< ub::matrix<double> > _AOatgridsub = ub::subrange(AOatgrid, (*_row)->getStartIndex(), (*_row)->getStartIndex()+(*_row)->getNumFunc(), 0, 0);
                        (*_row)->EvalAOspace(_AOatgridsub, _grid[i][j].grid_x, _grid[i][j].grid_y, _grid[i][j].grid_z);
                        
                        // for density gradient  
                        ub::matrix_range< ub::matrix<double> > _AODerXatgridsub = ub::subrange(AODerXatgrid, (*_row)->getStartIndex(), (*_row)->getStartIndex()+(*_row)->getNumFunc(), 0, 0);
                        ub::matrix_range< ub::matrix<double> > _AODerYatgridsub = ub::subrange(AODerYatgrid, (*_row)->getStartIndex(), (*_row)->getStartIndex()+(*_row)->getNumFunc(), 0, 0);
                        ub::matrix_range< ub::matrix<double> > _AODerZatgridsub = ub::subrange(AODerZatgrid, (*_row)->getStartIndex(), (*_row)->getStartIndex()+(*_row)->getNumFunc(), 0, 0);
                        (*_row)->EvalAOGradspace(_AODerXatgridsub,_AODerYatgridsub,_AODerZatgridsub, _grid[i][j].grid_x, _grid[i][j].grid_y, _grid[i][j].grid_z); 
                        
                        
                    }
                    
                    
                    ub::matrix<double> _AOmatrix_at_grid = ub::prod(AOatgrid, ub::trans(AOatgrid));
                    
                    // density at grid point is sum of element-wise product of density matrix x _AOmatrix
                    ub::vector<double> _AO_array  =_AOmatrix_at_grid.data();
                    double density_at_grid = 0.0;
                    for ( int _i =0; _i < DMAT_array.size(); _i++ ){
                        density_at_grid += DMAT_array(_i)*_AO_array(_i);
                    }
                    
                    // density gradient as grad(n) = sum_{ab}[D_{ab} (X_b grad(X_a) + X_a grad(X_b)]
                    // x-component of gradient
                    ub::matrix<double> _AODerXmatrix_at_grid = ub::prod(AODerXatgrid, ub::trans(AOatgrid)) + ub::prod(AOatgrid, ub::trans(AODerXatgrid));
                    ub::matrix<double> _AODerYmatrix_at_grid = ub::prod(AODerYatgrid, ub::trans(AOatgrid)) + ub::prod(AOatgrid, ub::trans(AODerYatgrid));
                    ub::matrix<double> _AODerZmatrix_at_grid = ub::prod(AODerZatgrid, ub::trans(AOatgrid)) + ub::prod(AOatgrid, ub::trans(AODerZatgrid));
                    
                    ub::vector<double> _AODerX_array  =_AODerXmatrix_at_grid.data();
                    ub::vector<double> _AODerY_array  =_AODerYmatrix_at_grid.data();
                    ub::vector<double> _AODerZ_array  =_AODerZmatrix_at_grid.data();
                    
                    double densityDerX_at_grid = 0.0;
                    double densityDerY_at_grid = 0.0;
                    double densityDerZ_at_grid = 0.0;
                    for ( int _i =0; _i < DMAT_array.size(); _i++ ){
                        densityDerX_at_grid += DMAT_array(_i)*_AODerX_array(_i);
                        densityDerY_at_grid += DMAT_array(_i)*_AODerY_array(_i);
                        densityDerZ_at_grid += DMAT_array(_i)*_AODerZ_array(_i);
                    }
            
                    
                    
                    
                    // get XC for this density_at_grid
                    double f_xc; // E_xc[n] = int{n(r)*eps_xc[n(r)] d3r} = int{ f_xc(r) d3r }
                    double v_xc; // v_xc(r)
                    double dv_drho_dX;
                    double dv_drho_dY;
                    double dv_drho_dZ;
                     _xc.getXC("PBE", density_at_grid, densityDerX_at_grid, densityDerY_at_grid, densityDerZ_at_grid, f_xc , v_xc, dv_drho_dX,dv_drho_dY, dv_drho_dZ );



                    double vsigma[1]; // output of libxc call 
                    double vrho[1]; // output of libxc call

                    if (0 == 0) {
                        v_xc = 0.0;
                        double sigma_at_grid = densityDerX_at_grid * densityDerX_at_grid + densityDerY_at_grid * densityDerY_at_grid + densityDerZ_at_grid*densityDerZ_at_grid;

                        // via libxc xchange part only
                        switch (xfunc.info->family) {
                            case XC_FAMILY_LDA:
                                xc_lda_vxc(&xfunc, 1, &density_at_grid, vrho);
                                break;
                            case XC_FAMILY_GGA:
                            case XC_FAMILY_HYB_GGA:
                                xc_gga_vxc(&xfunc, 1, &density_at_grid, &sigma_at_grid, vrho, vsigma);
                                break;
                        }
                        v_xc = vrho[0];

                        cout << " XClib exchange " << (*vrho) << endl;

                        // via libxc correlation part only
                        switch (cfunc.info->family) {
                            case XC_FAMILY_LDA:
                                xc_lda_vxc(&cfunc, 1, &density_at_grid, vrho);
                                break;
                            case XC_FAMILY_GGA:
                            case XC_FAMILY_HYB_GGA:
                                xc_gga_vxc(&cfunc, 1, &density_at_grid, &sigma_at_grid, vrho, vsigma);
                                break;
                        }

                        cout << " XClib correlation " << (*vrho) << endl;

                        v_xc += vrho[0];

                    }
                    
		     exit(0);
                    // cout << " out rho : " << density_at_grid << " vxc " << v << endl;
                    XCMAT += _grid[i][j].grid_weight * v_xc * _AOmatrix_at_grid;
                    

                    // result += _grid[i][j].grid_weight * density_at_grid;
                }
            } // gridpoints end
            
            return XCMAT;
            
        }

        
        // numerically integrate the electron density
        double NumericalIntegration::IntegrateDensity(ub::matrix<double>& _density_matrix, AOBasis* basis){
            
            double result = 0.0;
            const ub::vector<double> DMAT_array=_density_matrix.data();
             // for every gridpoint
            for (int i = 0; i < _grid.size(); i++) {
                for (int j = 0; j < _grid[i].size(); j++) {
                    // get value of orbitals at each gridpoint
                    ub::matrix<double> tmat = ub::zero_matrix<double>(basis->_AOBasisSize, 1);

                    for (vector< AOShell* >::iterator _row = basis->firstShell(); _row != basis->lastShell(); _row++) {

                        ub::matrix_range< ub::matrix<double> > _submatrix = ub::subrange(tmat, (*_row)->getStartIndex(), (*_row)->getStartIndex()+(*_row)->getNumFunc(), 0, 0);
                        (*_row)->EvalAOspace(_submatrix, _grid[i][j].grid_x, _grid[i][j].grid_y, _grid[i][j].grid_z);
                    }
                    
                    
                    ub::matrix<double> _AOmatrix_at_grid = ub::prod(tmat, ub::trans(tmat));
                    
                    // density at grid point is sum of element-wise product of density matrix x _AOmatrix
                    ub::vector<double> _AO_array  =_AOmatrix_at_grid.data();
                    double density_at_grid = 0.0;
                    for ( int _i =0; _i < DMAT_array.size(); _i++ ){
                        density_at_grid += DMAT_array(_i)*_AO_array(_i);
                    }   
                    

                    
                    
                    result += _grid[i][j].grid_weight * density_at_grid;
                }
            } // gridpoints end
            
            
            return result;
            
            
            
            
        }
        
        
        
        
        
        
        // numerically integrate the elements of the AOOverlap matrix as check

        ub::matrix<double> NumericalIntegration::numAOoverlap(AOBasis* basis) {


            ub::matrix<double> OLMAT = ub::zero_matrix<double>(basis->_AOBasisSize, basis->_AOBasisSize);
            // for every gridpoint
            for (int i = 0; i < _grid.size(); i++) {
                for (int j = 0; j < _grid[i].size(); j++) {
                    // get value of orbitals at each gridpoint
                    ub::matrix<double> tmat = ub::zero_matrix<double>(basis->_AOBasisSize, 1);

                    for (vector< AOShell* >::iterator _row = basis->firstShell(); _row != basis->lastShell(); _row++) {

                        ub::matrix_range< ub::matrix<double> > _submatrix = ub::subrange(tmat, (*_row)->getStartIndex(), (*_row)->getStartIndex()+(*_row)->getNumFunc(), 0, 0);
                        (*_row)->EvalAOspace(_submatrix, _grid[i][j].grid_x, _grid[i][j].grid_y, _grid[i][j].grid_z);
                    }

                    OLMAT += _grid[i][j].grid_weight * ub::prod(tmat, ub::trans(tmat));
                }
            } // gridpoints end


            return OLMAT;

        } // numAOoverlap

        
        
        
        
        
        
        
        
        
        
        
        double NumericalIntegration::StupidIntegrate(std::vector<double>& _data){
            
            
            double integral = 0.0;
            int _i_point = 0;
            for ( int i = 0 ; i < _grid.size(); i++){
                for ( int j = 0 ; j < _grid[i].size(); j++){

                    
                    integral += _data[_i_point] * _grid[i][j].grid_weight;
                    
                    _i_point++;

                }
            }
            
            return integral;
            
        }
        
        
        
        void NumericalIntegration::getGridpoints( ub::matrix<double>& _gridpoints ){
            
            _gridpoints = ub::zero_matrix<double>(_totalgridsize,4);
            
            int _i_point = 0;
            for ( int i = 0 ; i < _grid.size(); i++){
                for ( int j = 0 ; j < _grid[i].size(); j++){
                    
                    _gridpoints(_i_point,0) = _grid[i][j].grid_x;
                    _gridpoints(_i_point,1) = _grid[i][j].grid_y;
                    _gridpoints(_i_point,2) = _grid[i][j].grid_z;
                    _gridpoints(_i_point,3) = _grid[i][j].grid_weight;
                    
                    _i_point++;
                }
                }
            
            
            
            
        }
        
        
        
        
        void NumericalIntegration::GridSetup(string type, BasisSet* bs, vector<QMAtom*> _atoms) {

            const double pi = boost::math::constants::pi<double>();
            // get GridContainer
            GridContainers _grids;

            // get radial grid per element
            EulerMaclaurinGrid _radialgrid;
            _radialgrid.getRadialGrid(bs, _atoms, "medium", _grids); // this checks out 1:1 with NWChem results! AWESOME

            cout << "Radial grid summary " << endl;
            map<string, GridContainers::radial_grid>::iterator it;
            for (it = _grids._radial_grids.begin(); it != _grids._radial_grids.end(); ++it) {
                cout << " Element " << it->first << " Number of points " << it->second.radius.size() << endl;
            }

            // get angular grid per element
            LebedevGrid _sphericalgrid;
            cout << "Spherical grid summary " << endl;
            for (it = _grids._radial_grids.begin(); it != _grids._radial_grids.end(); ++it) {
                _sphericalgrid.getSphericalGrid(_atoms, "medium", _grids);
                cout << " Element " << it->first << " Number of points " << _grids._spherical_grids.at(it->first).weight.size() << endl;

            }

            
            // for the partitioning, we need all inter-center distances later, stored in one-directional list
            int ij = 0;
            Rij.push_back(0.0); // 1st center "self-distance"
            Rij_mat = ub::zero_matrix<double>(_atoms.size(),_atoms.size());
            vector< QMAtom* > ::iterator ait;
            vector< QMAtom* > ::iterator bit;
            int i = 1;
            for (ait = _atoms.begin() + 1; ait != _atoms.end(); ++ait) {
                // get center coordinates in Bohr
                double x_a = (*ait)->x * ang2bohr;
                double y_a = (*ait)->y * ang2bohr;
                double z_a = (*ait)->z * ang2bohr;
                int j = 0;
                for (bit = _atoms.begin(); bit != ait; ++bit) {
                    ij++;
                    // get center coordinates in Bohr
                    double x_b = (*bit)->x * ang2bohr;
                    double y_b = (*bit)->y * ang2bohr;
                    double z_b = (*bit)->z * ang2bohr;

                    Rij.push_back(1.0 / sqrt((x_a - x_b)*(x_a - x_b) + (y_a - y_b)*(y_a - y_b) + (z_a - z_b)*(z_a - z_b)));


                    Rij_mat(i,j) = 1.0 / sqrt((x_a - x_b)*(x_a - x_b) + (y_a - y_b)*(y_a - y_b) + (z_a - z_b)*(z_a - z_b));

                    cout << i << " " << j << " : " << Rij_mat(i,j) << endl;
                    
                    j++;
                } // atoms
                Rij.push_back(0.0); // self-distance again
                i++;
            } // atoms
            
            //cout << " Determined all inter-center distances " << endl;
            

            // combine the element-based information with the geometry
        

            int i_atom = 0;
            _totalgridsize = 0;
            for (ait = _atoms.begin(); ait < _atoms.end(); ++ait) {
                // get center coordinates in Bohr
                std::vector< GridContainers::integration_grid > _atomgrid;
                double x_c = (*ait)->x * ang2bohr;
                double y_c = (*ait)->y * ang2bohr;
                double z_c = (*ait)->z * ang2bohr;
                string name = (*ait)->type;

                // get radial grid information for this atom type
                GridContainers::radial_grid _radial_grid = _grids._radial_grids.at(name);

                // cout << " Loaded radial grid of atom " << i_atom << endl;

                
                // get spherical grid information for this atom type
                GridContainers::spherical_grid _spherical_grid = _grids._spherical_grids.at(name);

                // cout << " Loaded spherical grid of atom " << i_atom << endl;
            

                // for each radial value
                for (int _i_rad = 0; _i_rad < _radial_grid.radius.size(); _i_rad++) {
                    double r = _radial_grid.radius[_i_rad];
                    // for each (theta,phi)
                    for (int _i_sph = 0; _i_sph < _spherical_grid.phi.size(); _i_sph++) {

                        double p = _spherical_grid.phi[_i_sph] * pi / 180.0; // back to rad
                        double t = _spherical_grid.theta[_i_sph] * pi / 180.0; // back to rad
                        double ws = _spherical_grid.weight[_i_sph];

                        double x_s = sin(p) * cos(t);
                        double y_s = sin(p) * sin(t);
                        double z_s = cos(p);


                        GridContainers::integration_grid _gridpoint;
                        _gridpoint.grid_x = x_c + r * x_s;
                        _gridpoint.grid_y = y_c + r * y_s;
                        _gridpoint.grid_z = z_c + r * z_s;

                        _gridpoint.grid_weight = _radial_grid.weight[_i_rad] * ws;

                        _atomgrid.push_back(_gridpoint);


                    } // spherical gridpoints
                } // radial gridpoint

                // cout << " Constructed full grid of atom " << i_atom << " of size " << _atomgrid.size() <<  endl;
                
                int fullsize = _atomgrid.size();
                
                
                if ( 0 == 0 ){
                
                // now the partition function magic for this _atomgrid
                // some parameters
                double eps = 0.002;
                double ass = 0.725;
                
                // get all distances from grid points to centers
                std::vector< std::vector<double> > rq;
                // for each center
                for (bit = _atoms.begin(); bit < _atoms.end(); ++bit) {
                    // get center coordinates
                    double x_b = (*bit)->x * ang2bohr;
                    double y_b = (*bit)->y * ang2bohr;
                    double z_b = (*bit)->z * ang2bohr;

                    std::vector<double> temp;
                    // for each gridpoint
                    for (std::vector<GridContainers::integration_grid >::iterator git = _atomgrid.begin(); git != _atomgrid.end(); ++git) {

                        double x = (*git).grid_x - x_b;
                        double y = (*git).grid_y - y_b;
                        double z = (*git).grid_z - z_b;

                        temp.push_back(sqrt(x * x + y * y + z * z));

                    } // gridpoint of _atomgrid
                    rq.push_back(temp); // rq[center][gridpoint]

                } // centers
                // cout << " Calculated all gridpoint distances to centers for " << i_atom << endl;
                
                // find nearest-neighbor of this atom
                double distNN = 1e10;

                vector< QMAtom* > ::iterator NNit;
                int i_NN;

                // now check all other centers
                int i_b =0;
                for (bit = _atoms.begin(); bit != _atoms.end(); ++bit) {

                    if (bit != ait) {
                        // get center coordinates
                        double x_b = (*bit)->x * ang2bohr;
                        double y_b = (*bit)->y * ang2bohr;
                        double z_b = (*bit)->z * ang2bohr;

                        double distSQ = (x_c - x_b)*(x_c - x_b) + (y_c - y_b)*(y_c - y_b) + (z_c - z_b)*(z_c - z_b);

                        // update NN distance and iterator
                        if ( distSQ < distNN ) {
                            distNN = distSQ;
                            NNit = bit;
                            i_NN = i_b;
                        }

                    } // if ( ait != bit) 
                    i_b++;
                }// bit centers
                 // cout << " Nearest neighbor of atom " << i_atom << " is atom " << i_NN << " at distance " << distNN << endl;
                
                double radwgh = (1.0 - ass ) * sqrt(distNN) * 0.5;
                /* according to SSW scheme, all gridpoints within radwgh 
                 * of its parent center have weight one, and we can skip
                 * calculating the weighting function explicitly.
                 * Since the gridpoints in _atomgrid are sorted with increasing
                 * distance from the center, we can go through the list easily
                 */
                
             /*   int _idx_left = 0;
                for ( int i_grid  = 0 ; i_grid < _atomgrid.size(); i_grid++) {
                    if ( rq[i_atom][i_grid] > (radwgh + eps)  ) {
                        _idx_left = i_grid;
                        break; // out of the for-loop
                    }
                    i_grid++;
                } */
                
                //cout << " First forward non-unity weight is for gridpoint " << _idx_left << endl;
                
                /* Similarly, all gridpoints g for which 
                 * 
                 *      mu_ij = (r_ig - r_jg)/R_ij > a
                 *   
                 *   for i = parent atom and j = next neighbor
                 * 
                 * have zero weight. So we start from the end of the 
                 * gridpoint list and set all weights to zero until 
                 * we find the first non-zero contribution.
                 */

                // update NN distance
             /*   distNN = (ass-eps) * sqrt(distNN) ;
                // reduce "checklist" backward
                int _idx_right;
                for (int i_grid = _atomgrid.size()-1; i_grid >= _idx_left; i_grid--) {
                    cout << i_grid << "  is " <<  rq[i_atom][i_grid] - rq[i_NN][i_grid] << " vs " << distNN << endl;
                    
                    if (  (rq[i_atom][i_grid] - rq[i_NN][i_grid] ) > distNN   ) {
                        // set weight to zero
                        _atomgrid[i_grid].grid_weight = 0.0;
                    } else {
                        _idx_right = i_grid;
                        break;
                    }
                } // right index
                cout << " First backward non-zero weight is for gridpoint " << _idx_right << endl; */
                
                /* only for the remaining gridpoint [_idx_left:_idx_right], we
                 * have to evaluate the weights explicitly
                 */
                //for ( int i_grid = _idx_left; i_grid <= _idx_right ; i_grid++){
                for ( int i_grid = 0; i_grid < _atomgrid.size() ; i_grid++){
                    //cout << " modifying point " << i_grid << endl;
                    // call some shit called grid_ssw0 in NWChem
                    std::vector<double> _p = SSWpartition( _atomgrid.size(), i_grid, _atoms.size(),rq, ass );
                    //cout << " partition for gridpoint " << i_grid << endl;
                    // check weight sum
                    double wsum = 0.0;
                    for (int i =0 ; i < _p.size(); i++ ){
                        wsum += _p[i];
                    }
                    //cout << " sum of partition weights " << wsum << endl;
                    if ( wsum != 0.0 ){
                        
                        // update the weight of this grid point
                        _atomgrid[i_grid].grid_weight = _atomgrid[i_grid].grid_weight * _p[i_atom]/wsum;
                        //cout << " adjusting gridpoint weight "  << endl;
                    } else {
                        
                       cerr << "\nSum of partition weights of grid point " << i_grid << " of atom " << i_atom << " is zero! ";
                       throw std::runtime_error("\nThis should never happen!"); 
                        
                    }
                    

                } // partition weight for each gridpoint

                // now remove points from the grid with negligible weights
                
                for (std::vector<GridContainers::integration_grid >::iterator git = _atomgrid.begin(); git != _atomgrid.end();) {
                    if (git->grid_weight < 1e-13 ) {
                        git = _atomgrid.erase(git);
                    } else {
                        ++git;
                    }
                }
                
                } // 1 == 0
                
                cout << " Total size of integration grid for atom: " << i_atom << " : " << _atomgrid.size() << " from " << fullsize << endl;

                _totalgridsize += _atomgrid.size() ;
                _grid.push_back(_atomgrid);
                i_atom++;
            } // atoms


            
            ofstream points;
            points.open("molgrid.xyz", ofstream::out);
            points << _totalgridsize << endl;
            points << endl;
            for ( int i = 0 ; i < _grid.size(); i++){
                for ( int j = 0 ; j < _grid[i].size(); j++){
                points << "X " << _grid[i][j].grid_x/ang2bohr << " " << _grid[i][j].grid_y/ang2bohr << " " << _grid[i][j].grid_z/ang2bohr << " "  << _grid[i][j].grid_weight << endl;
                }
            }
            points.close();


        }

        
        
        std::vector<double> NumericalIntegration::SSWpartition(int ngrid, int igrid, int ncenters, std::vector< std::vector<double> >& rq,  double ass){
            
            // initialize partition vector to 1.0
            std::vector<double> p(ncenters,1.0);
            
            const double tol_scr = 1e-10;
            const double leps    = 1e-6; 
            // go through centers
            for ( int i = 1; i < ncenters; i++ ){
                
                int ij = i*(i+1)/2 -1; // indexing magic
                double rag = rq[i][igrid] ;
                
                // through all other centers (one-directional)
                for (int j = 0; j < i; j++ ){
                    
                    ij++;
                    if ( ( std::abs(p[i]) > tol_scr  ) || ( std::abs(p[j]) > tol_scr  ) ){
                        
                        // cout << "Rij " <<  Rij_mat(i,j) << " or " << Rij[ij] << endl;
                        
                        double mu = ( rag - rq[j][igrid] )*Rij[ij]; 
                        if ( mu > ass ) {
                            p[i] = 0.0;
                        } else if ( mu < -ass ) {
                            p[j] = 0.0;
                        } else {
                            
                            double sk;
                            if (std::abs(mu) < leps ) {
                                sk = -1.88603178008*mu + 0.5;
                            } else {
                                sk = erf1c(mu); 
                            }
                            if ( mu > 0.0 ) sk = 1.0 - sk;
                            p[j] = p[j] * sk;
                            p[i] = p[i] * (1.0-sk);
                            
                            
                        }
                        
                    }
                    
                    
                }
                
                
                
                
            }
            
            return p;
            
            
            
            
            
        }

        

        double NumericalIntegration::erf1c(double x){
             
            
            return 0.5*erfcc((x/(1.0-x*x))*alpha_erf1);    
            
        }
        
        
        double NumericalIntegration::erfcc(double x){
            
            double tau = 1.0/(1.0+0.5*std::abs(x));
            
            return tau*exp(-x*x-1.26551223 + 1.00002368*tau + 0.37409196*tau*tau 
            + 0.09678418*pow(tau,3) - 0.18628806*pow(tau,4) + 0.27886807*pow(tau,5) 
            -1.13520398*pow(tau,6) + 1.48851587*pow(tau,7)  -0.82215223*pow(tau,8) 
            + 0.17087277*pow(tau,9));
            
        }
                                                                                                

        
        
        
        



    }
}
