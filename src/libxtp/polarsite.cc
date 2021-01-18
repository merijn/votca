/*
 *           Copyright 2009-2020 The VOTCA Development Team
 *                      (http://www.votca.org)
 *
 *     Licensed under the Apache License,Version 2.0 (the "License")
 *
 *You may not use this file except in compliance with the License.
 *You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,software
 *distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,either express or implied.
 *See the License for the specific language governing permissions and
 *limitations under the License.
 *
 */

// Standard includes
#include <fstream>
#include <string>

// Third party includes
#include <boost/format.hpp>

// VOTCA includes
#include <votca/tools/constants.h>

// Local VOTCA includes
#include "votca/xtp/polarsite.h"
#include  "votca/xtp/checkpointtable.h"


using namespace std;

namespace votca {
namespace xtp {

PolarSite::PolarSite(Index id, std::string element, Eigen::Vector3d pos)
    : StaticSite(id, element, pos) {
  tools::Elements e;
  double default_pol = std::pow(tools::conv::ang2bohr, 3);
  try {
    default_pol =
        e.getPolarizability(element) * std::pow(tools::conv::nm2bohr, 3);
  } catch (const std::runtime_error&) {
    ;
  }
  setpolarization(default_pol * Eigen::Matrix3d::Identity());
}

PolarSite::PolarSite(const data& d) { ReadData(d); }

Eigen::Vector3d PolarSite::getDipole() const {
  Eigen::Vector3d dipole = _Q.segment<3>(1);
  dipole += Induced_Dipole();
  return dipole;
}

void PolarSite::setpolarization(const Eigen::Matrix3d& pol) {
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
  es.computeDirect(pol);
  _pinv = es.eigenvectors() * es.eigenvalues().cwiseInverse().asDiagonal() *
          es.eigenvectors().transpose();
  _eigendamp_invsqrt = 1.0 / std::sqrt(es.eigenvalues().maxCoeff());
}

std::string PolarSite::writepolarization() const {
  double conv_pol = std::pow(tools::conv::bohr2ang, 3);
  Eigen::MatrixX3d pol = _pinv.inverse() * conv_pol;
  return (boost::format("     P %1$+1.7f %2$+1.7f %3$+1.7f %4$+1.7f %5$+1.7f "
                        "%6$+1.7f\n") %
          pol(0, 0) % pol(1, 0) % pol(2, 0) % pol(1, 1) % pol(1, 2) % pol(2, 2))
      .str();
}

void PolarSite::SetupCptTable(CptTable& table)  {
  table.addCol<Index>("index", HOFFSET(data, id));
  table.addCol<std::string>( "type", HOFFSET(data, element));

  table.addCol<double>( "posX", HOFFSET(data, posX));
  table.addCol<double>( "posY", HOFFSET(data, posY));
  table.addCol<double>( "posZ", HOFFSET(data, posZ));
  // same type as rank
  table.addCol<Index>("rank", HOFFSET(data, rank));

  table.addCol<double>("Q00", HOFFSET(data, Q00));
  table.addCol<double>( "Q11c", HOFFSET(data, Q11c));
  table.addCol<double>( "Q11s", HOFFSET(data, Q11s));
  table.addCol<double>( "Q10", HOFFSET(data, Q10));
  table.addCol<double>( "Q20", HOFFSET(data, Q20));
  table.addCol<double>("Q21c", HOFFSET(data, Q21c));
  table.addCol<double>( "Q21s", HOFFSET(data, Q21s));
  table.addCol<double>( "Q22c", HOFFSET(data, Q22c));
  table.addCol<double>("Q22s", HOFFSET(data, Q22s));

  table.addCol<double>( "Vx", HOFFSET(data, Vx));
  table.addCol<double>( "Vy", HOFFSET(data, Vy));
  table.addCol<double>( "Vz", HOFFSET(data, Vz));

  table.addCol<double>( "Vx_noE", HOFFSET(data, Vx_noE));
  table.addCol<double>( "Vy_NoE", HOFFSET(data, Vy_noE));
  table.addCol<double>( "Vz_noE", HOFFSET(data, Vz_noE));
  // P_inv and P have same polarization so no problem, as only data type counts
  table.addCol<double>( "pxx", HOFFSET(data, pxx));
  table.addCol<double>( "pxy", HOFFSET(data, pxy));
  table.addCol<double>("pxz", HOFFSET(data, pxz));
  table.addCol<double>( "pyy", HOFFSET(data, pyy));
  table.addCol<double>( "pyz", HOFFSET(data, pyz));
  table.addCol<double>("pzz", HOFFSET(data, pzz));

  table.addCol<double>( "d_ind_x", HOFFSET(data, d_x_ind));
  table.addCol<double>( "d_ind_y", HOFFSET(data, d_y_ind));
  table.addCol<double>( "d_ind_z", HOFFSET(data, d_z_ind));
}

void PolarSite::WriteData(data& d) const {
  d.id = _id;
  d.element = const_cast<char*>(_element.c_str());
  d.posX = _pos[0];
  d.posY = _pos[1];
  d.posZ = _pos[2];

  d.rank = _rank;

  d.Q00 = _Q[0];
  d.Q11c = _Q[1];
  d.Q11s = _Q[2];
  d.Q10 = _Q[3];
  d.Q20 = _Q[4];
  d.Q21c = _Q[5];
  d.Q21s = _Q[6];
  d.Q22c = _Q[7];
  d.Q22s = _Q[8];

  d.Vx = _V[0];
  d.Vy = _V[1];
  d.Vz = _V[2];

  d.Vx_noE = _V_noE[0];
  d.Vy_noE = _V_noE[1];
  d.Vz_noE = _V_noE[2];

  Eigen::Matrix3d P = _pinv.inverse();
  d.pxx = P(0, 0);
  d.pxy = P(0, 1);
  d.pxz = P(0, 2);
  d.pyy = P(1, 1);
  d.pyz = P(1, 2);
  d.pzz = P(2, 2);

  d.d_x_ind = _induced_dipole.x();
  d.d_y_ind = _induced_dipole.y();
  d.d_z_ind = _induced_dipole.z();
}

void PolarSite::ReadData(const data& d) {
  _id = d.id;
  _element = std::string(d.element);
  free(d.element);
  _pos[0] = d.posX;
  _pos[1] = d.posY;
  _pos[2] = d.posZ;

  _rank = d.rank;

  _Q[0] = d.Q00;
  _Q[1] = d.Q11c;
  _Q[2] = d.Q11s;
  _Q[3] = d.Q10;
  _Q[4] = d.Q20;
  _Q[5] = d.Q21c;
  _Q[6] = d.Q21s;
  _Q[7] = d.Q22c;
  _Q[8] = d.Q22s;

  _V[0] = d.Vx;
  _V[1] = d.Vy;
  _V[2] = d.Vz;

  _V_noE[0] = d.Vx_noE;
  _V_noE[1] = d.Vy_noE;
  _V_noE[2] = d.Vz_noE;

  Eigen::Matrix3d Ps;
  Ps(0, 0) = d.pxx;
  Ps(0, 1) = d.pxy;
  Ps(1, 0) = d.pxy;
  Ps(0, 2) = d.pxz;
  Ps(2, 0) = d.pxz;
  Ps(1, 1) = d.pyy;
  Ps(1, 2) = d.pyz;
  Ps(2, 1) = d.pyz;
  Ps(2, 2) = d.pzz;

  this->setpolarization(Ps);

  _induced_dipole.x() = d.d_x_ind;
  _induced_dipole.y() = d.d_y_ind;
  _induced_dipole.z() = d.d_z_ind;
}

}  // namespace xtp
}  // namespace votca
