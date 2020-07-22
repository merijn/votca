/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE aomatrix3d_test

// Third party includes
#include <boost/test/unit_test.hpp>

// Local VOTCA includes
#include "votca/xtp/aomatrix3d.h"
#include "votca/xtp/orbitals.h"

using namespace votca::xtp;
using namespace votca;
using namespace std;

BOOST_AUTO_TEST_SUITE(aomatrix3d_test)

BOOST_AUTO_TEST_CASE(aomatrices3d_test) {

  ofstream xyzfile("molecule.xyz");
  xyzfile << " 5" << endl;
  xyzfile << " methane" << endl;
  xyzfile << " C            .000000     .000000     .000000" << endl;
  xyzfile << " H            .629118     .629118     .629118" << endl;
  xyzfile << " H           -.629118    -.629118     .629118" << endl;
  xyzfile << " H            .629118    -.629118    -.629118" << endl;
  xyzfile << " H           -.629118     .629118    -.629118" << endl;
  xyzfile.close();

  ofstream basisfile("3-21G.xml");
  basisfile << "<basis name=\"3-21G\">" << endl;
  basisfile << "  <element name=\"H\">" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << endl;
  basisfile << "      <constant decay=\"5.447178e+00\">" << endl;
  basisfile << "        <contractions factor=\"1.562850e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"8.245470e-01\">" << endl;
  basisfile << "        <contractions factor=\"9.046910e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << endl;
  basisfile << "      <constant decay=\"1.831920e-01\">" << endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "  </element>" << endl;
  basisfile << "  <element name=\"C\">" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << endl;
  basisfile << "      <constant decay=\"1.722560e+02\">" << endl;
  basisfile << "        <contractions factor=\"6.176690e-02\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"2.591090e+01\">" << endl;
  basisfile << "        <contractions factor=\"3.587940e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"5.533350e+00\">" << endl;
  basisfile << "        <contractions factor=\"7.007130e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"SP\">" << endl;
  basisfile << "      <constant decay=\"3.664980e+00\">" << endl;
  basisfile << "        <contractions factor=\"-3.958970e-01\" type=\"S\"/>"
            << endl;
  basisfile << "        <contractions factor=\"2.364600e-01\" type=\"P\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"7.705450e-01\">" << endl;
  basisfile << "        <contractions factor=\"1.215840e+00\" type=\"S\"/>"
            << endl;
  basisfile << "        <contractions factor=\"8.606190e-01\" type=\"P\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"SP\">" << endl;
  basisfile << "      <constant decay=\"1.958570e-01\">" << endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"S\"/>"
            << endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"P\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "  </element>" << endl;
  basisfile << "</basis>" << endl;
  basisfile.close();

  Orbitals orbitals;
  orbitals.QMAtoms().LoadFromFile("molecule.xyz");
  BasisSet basis;
  basis.Load("3-21G.xml");
  AOBasis aobasis;
  aobasis.Fill(basis, orbitals.QMAtoms());

  AODipole dip;
  dip.Fill(aobasis);

  Eigen::MatrixXd dip0_ref = Eigen::MatrixXd::Zero(17, 17);

  dip0_ref << 0, 0, 0, 0, 0.082882, 0, 0, 0, 0.0122969, 0.00265068, 0.00272642,
      -0.00265068, -0.00272642, 0.00265068, 0.00272642, -0.00265068,
      -0.00272642, 0, 0, 0, 0, 0.608935, 0, 0, 0, 0.388177, 0.127733, 0.10069,
      -0.127733, -0.10069, 0.127733, 0.10069, -0.127733, -0.10069, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0.104648, 0.029801, -0.104648, -0.029801, -0.104648,
      -0.029801, 0.104648, 0.029801, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.104648,
      0.029801, 0.104648, 0.029801, -0.104648, -0.029801, -0.104648, -0.029801,
      0.082882, 0.608935, 0, 0, 0, 0.597616, 0, 0, 0, 0.187889, 0.341144,
      0.187889, 0.341144, 0.187889, 0.341144, 0.187889, 0.341144, 0, 0, 0, 0,
      0.597616, 0, 0, 0, 1.1298, 0.328374, 0.3843, -0.328374, -0.3843, 0.328374,
      0.3843, -0.328374, -0.3843, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.282166, 0.195439,
      -0.282166, -0.195439, -0.282166, -0.195439, 0.282166, 0.195439, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0.282166, 0.195439, 0.282166, 0.195439, -0.282166,
      -0.195439, -0.282166, -0.195439, 0.0122969, 0.388177, 0, 0, 0, 1.1298, 0,
      0, 0, 0.423587, 0.976351, 0.423587, 0.976351, 0.423587, 0.976351,
      0.423587, 0.976351, 0.00265068, 0.127733, 0.104648, 0.104648, 0.187889,
      0.328374, 0.282166, 0.282166, 0.423587, 1.18886, 0.767884, -3.38813e-20,
      0.0901008, 0.00925314, 0.139089, -3.38813e-20, 0.0901008, 0.00272642,
      0.10069, 0.029801, 0.029801, 0.341144, 0.3843, 0.195439, 0.195439,
      0.976351, 0.767884, 1.18886, -0.0901008, 0, 0.139089, 0.422025,
      -0.0901008, 0, -0.00265068, -0.127733, -0.104648, 0.104648, 0.187889,
      -0.328374, -0.282166, 0.282166, 0.423587, 3.38813e-20, -0.0901008,
      -1.18886, -0.767884, 3.38813e-20, -0.0901008, -0.00925314, -0.139089,
      -0.00272642, -0.10069, -0.029801, 0.029801, 0.341144, -0.3843, -0.195439,
      0.195439, 0.976351, 0.0901008, 0, -0.767884, -1.18886, 0.0901008, 0,
      -0.139089, -0.422025, 0.00265068, 0.127733, -0.104648, -0.104648,
      0.187889, 0.328374, -0.282166, -0.282166, 0.423587, 0.00925314, 0.139089,
      -3.38813e-20, 0.0901008, 1.18886, 0.767884, -3.38813e-20, 0.0901008,
      0.00272642, 0.10069, -0.029801, -0.029801, 0.341144, 0.3843, -0.195439,
      -0.195439, 0.976351, 0.139089, 0.422025, -0.0901008, 0, 0.767884, 1.18886,
      -0.0901008, 0, -0.00265068, -0.127733, 0.104648, -0.104648, 0.187889,
      -0.328374, 0.282166, -0.282166, 0.423587, 3.38813e-20, -0.0901008,
      -0.00925314, -0.139089, 3.38813e-20, -0.0901008, -1.18886, -0.767884,
      -0.00272642, -0.10069, 0.029801, -0.029801, 0.341144, -0.3843, 0.195439,
      -0.195439, 0.976351, 0.0901008, 0, -0.139089, -0.422025, 0.0901008, 0,
      -0.767884, -1.18886;

  Eigen::MatrixXd dip1_ref = Eigen::MatrixXd::Zero(17, 17);
  dip1_ref << 0, 0, 0, 0.082882, 0, 0, 0, 0.0122969, 0, 0.00265068, 0.00272642,
      -0.00265068, -0.00272642, -0.00265068, -0.00272642, 0.00265068,
      0.00272642, 0, 0, 0, 0.608935, 0, 0, 0, 0.388177, 0, 0.127733, 0.10069,
      -0.127733, -0.10069, -0.127733, -0.10069, 0.127733, 0.10069, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0.104648, 0.029801, -0.104648, -0.029801, 0.104648,
      0.029801, -0.104648, -0.029801, 0.082882, 0.608935, 0, 0, 0, 0.597616, 0,
      0, 0, 0.187889, 0.341144, 0.187889, 0.341144, 0.187889, 0.341144,
      0.187889, 0.341144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.104648, 0.029801,
      0.104648, 0.029801, -0.104648, -0.029801, -0.104648, -0.029801, 0, 0, 0,
      0.597616, 0, 0, 0, 1.1298, 0, 0.328374, 0.3843, -0.328374, -0.3843,
      -0.328374, -0.3843, 0.328374, 0.3843, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.282166,
      0.195439, -0.282166, -0.195439, 0.282166, 0.195439, -0.282166, -0.195439,
      0.0122969, 0.388177, 0, 0, 0, 1.1298, 0, 0, 0, 0.423587, 0.976351,
      0.423587, 0.976351, 0.423587, 0.976351, 0.423587, 0.976351, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0.282166, 0.195439, 0.282166, 0.195439, -0.282166, -0.195439,
      -0.282166, -0.195439, 0.00265068, 0.127733, 0.104648, 0.187889, 0.104648,
      0.328374, 0.282166, 0.423587, 0.282166, 1.18886, 0.767884, -3.38813e-20,
      0.0901008, -3.38813e-20, 0.0901008, 0.00925314, 0.139089, 0.00272642,
      0.10069, 0.029801, 0.341144, 0.029801, 0.3843, 0.195439, 0.976351,
      0.195439, 0.767884, 1.18886, -0.0901008, 0, -0.0901008, 0, 0.139089,
      0.422025, -0.00265068, -0.127733, -0.104648, 0.187889, 0.104648,
      -0.328374, -0.282166, 0.423587, 0.282166, 3.38813e-20, -0.0901008,
      -1.18886, -0.767884, -0.00925314, -0.139089, 3.38813e-20, -0.0901008,
      -0.00272642, -0.10069, -0.029801, 0.341144, 0.029801, -0.3843, -0.195439,
      0.976351, 0.195439, 0.0901008, 0, -0.767884, -1.18886, -0.139089,
      -0.422025, 0.0901008, 0, -0.00265068, -0.127733, 0.104648, 0.187889,
      -0.104648, -0.328374, 0.282166, 0.423587, -0.282166, 3.38813e-20,
      -0.0901008, -0.00925314, -0.139089, -1.18886, -0.767884, 3.38813e-20,
      -0.0901008, -0.00272642, -0.10069, 0.029801, 0.341144, -0.029801, -0.3843,
      0.195439, 0.976351, -0.195439, 0.0901008, 0, -0.139089, -0.422025,
      -0.767884, -1.18886, 0.0901008, 0, 0.00265068, 0.127733, -0.104648,
      0.187889, -0.104648, 0.328374, -0.282166, 0.423587, -0.282166, 0.00925314,
      0.139089, -3.38813e-20, 0.0901008, -3.38813e-20, 0.0901008, 1.18886,
      0.767884, 0.00272642, 0.10069, -0.029801, 0.341144, -0.029801, 0.3843,
      -0.195439, 0.976351, -0.195439, 0.139089, 0.422025, -0.0901008, 0,
      -0.0901008, 0, 0.767884, 1.18886;

  Eigen::MatrixXd dip2_ref = Eigen::MatrixXd::Zero(17, 17);

  dip2_ref << 0, 0, 0.082882, 0, 0, 0, 0.0122969, 0, 0, 0.00265068, 0.00272642,
      0.00265068, 0.00272642, -0.00265068, -0.00272642, -0.00265068,
      -0.00272642, 0, 0, 0.608935, 0, 0, 0, 0.388177, 0, 0, 0.127733, 0.10069,
      0.127733, 0.10069, -0.127733, -0.10069, -0.127733, -0.10069, 0.082882,
      0.608935, 0, 0, 0, 0.597616, 0, 0, 0, 0.187889, 0.341144, 0.187889,
      0.341144, 0.187889, 0.341144, 0.187889, 0.341144, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0.104648, 0.029801, -0.104648, -0.029801, 0.104648, 0.029801,
      -0.104648, -0.029801, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.104648, 0.029801,
      -0.104648, -0.029801, -0.104648, -0.029801, 0.104648, 0.029801, 0, 0,
      0.597616, 0, 0, 0, 1.1298, 0, 0, 0.328374, 0.3843, 0.328374, 0.3843,
      -0.328374, -0.3843, -0.328374, -0.3843, 0.0122969, 0.388177, 0, 0, 0,
      1.1298, 0, 0, 0, 0.423587, 0.976351, 0.423587, 0.976351, 0.423587,
      0.976351, 0.423587, 0.976351, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.282166,
      0.195439, -0.282166, -0.195439, 0.282166, 0.195439, -0.282166, -0.195439,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0.282166, 0.195439, -0.282166, -0.195439,
      -0.282166, -0.195439, 0.282166, 0.195439, 0.00265068, 0.127733, 0.187889,
      0.104648, 0.104648, 0.328374, 0.423587, 0.282166, 0.282166, 1.18886,
      0.767884, 0.00925314, 0.139089, -3.38813e-20, 0.0901008, -3.38813e-20,
      0.0901008, 0.00272642, 0.10069, 0.341144, 0.029801, 0.029801, 0.3843,
      0.976351, 0.195439, 0.195439, 0.767884, 1.18886, 0.139089, 0.422025,
      -0.0901008, 0, -0.0901008, 0, 0.00265068, 0.127733, 0.187889, -0.104648,
      -0.104648, 0.328374, 0.423587, -0.282166, -0.282166, 0.00925314, 0.139089,
      1.18886, 0.767884, -3.38813e-20, 0.0901008, -3.38813e-20, 0.0901008,
      0.00272642, 0.10069, 0.341144, -0.029801, -0.029801, 0.3843, 0.976351,
      -0.195439, -0.195439, 0.139089, 0.422025, 0.767884, 1.18886, -0.0901008,
      0, -0.0901008, 0, -0.00265068, -0.127733, 0.187889, 0.104648, -0.104648,
      -0.328374, 0.423587, 0.282166, -0.282166, 3.38813e-20, -0.0901008,
      3.38813e-20, -0.0901008, -1.18886, -0.767884, -0.00925314, -0.139089,
      -0.00272642, -0.10069, 0.341144, 0.029801, -0.029801, -0.3843, 0.976351,
      0.195439, -0.195439, 0.0901008, 0, 0.0901008, 0, -0.767884, -1.18886,
      -0.139089, -0.422025, -0.00265068, -0.127733, 0.187889, -0.104648,
      0.104648, -0.328374, 0.423587, -0.282166, 0.282166, 3.38813e-20,
      -0.0901008, 3.38813e-20, -0.0901008, -0.00925314, -0.139089, -1.18886,
      -0.767884, -0.00272642, -0.10069, 0.341144, -0.029801, 0.029801, -0.3843,
      0.976351, -0.195439, 0.195439, 0.0901008, 0, 0.0901008, 0, -0.139089,
      -0.422025, -0.767884, -1.18886;

  bool check_dip0 = dip.Matrix()[0].isApprox(dip0_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_dip0, 1);
  if (!check_dip0) {
    cout << "ref" << endl;
    cout << dip0_ref << endl;
    cout << "result" << endl;
    cout << dip.Matrix()[0] << endl;
  }
  bool check_dip1 = dip.Matrix()[1].isApprox(dip1_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_dip1, 1);
  if (!check_dip1) {
    cout << "ref" << endl;
    cout << dip1_ref << endl;
    cout << "result" << endl;
    cout << dip.Matrix()[1] << endl;
  }
  bool check_dip2 = dip.Matrix()[2].isApprox(dip2_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_dip2, 1);
  if (!check_dip2) {
    cout << "ref" << endl;
    cout << dip2_ref << endl;
    cout << "result" << endl;
    cout << dip.Matrix()[2] << endl;
  }

  AOMomentum momentum;
  momentum.Fill(aobasis);

  Eigen::MatrixXd momentum0_ref = Eigen::MatrixXd::Zero(17, 17);

  momentum0_ref << 0, 0, 0, 0, 1.12967, 0, 0, 0, 0.154782, 0.0328303, 0.0342226,
      -0.0328303, -0.0342226, 0.0328303, 0.0342226, -0.0328303, -0.0342226, 0,
      0, 0, 0, 0.539423, 0, 0, 0, 0.521834, 0.179053, 0.13797, -0.179053,
      -0.13797, 0.179053, 0.13797, -0.179053, -0.13797, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0.170183, 0.0481524, -0.170183, -0.0481524, -0.170183, -0.0481524,
      0.170183, 0.0481524, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.170183, 0.0481524,
      0.170183, 0.0481524, -0.170183, -0.0481524, -0.170183, -0.0481524,
      -1.12967, -0.539423, 0, 0, 0, -0.234095, 0, 0, 0, 0.0275385, -0.0659188,
      0.0275385, -0.0659188, 0.0275385, -0.0659188, 0.0275385, -0.0659188, 0, 0,
      0, 0, 0.234095, 0, 0, 0, 0.442557, 0.128629, 0.150536, -0.128629,
      -0.150536, 0.128629, 0.150536, -0.128629, -0.150536, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0.110528, 0.0765563, -0.110528, -0.0765563, -0.110528, -0.0765563,
      0.110528, 0.0765563, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.110528, 0.0765563,
      0.110528, 0.0765563, -0.110528, -0.0765563, -0.110528, -0.0765563,
      -0.154782, -0.521834, 0, 0, 0, -0.442557, 0, 0, 0, -0.133948, -0.209557,
      -0.133948, -0.209557, -0.133948, -0.209557, -0.133948, -0.209557,
      -0.0328303, -0.179053, -0.170183, -0.170183, -0.0275385, -0.128629,
      -0.110528, -0.110528, 0.133948, -2.18791e-16, -4.98587e-17, -0.0153284,
      -0.0839716, -6.65767e-20, -9.1556e-18, -0.0153284, -0.0839716, -0.0342226,
      -0.13797, -0.0481524, -0.0481524, 0.0659188, -0.150536, -0.0765563,
      -0.0765563, 0.209557, -2.24414e-16, 0, -0.0839716, -0.154623,
      -4.12093e-17, 0, -0.0839716, -0.154623, 0.0328303, 0.179053, 0.170183,
      -0.170183, -0.0275385, 0.128629, 0.110528, -0.110528, 0.133948, 0.0153284,
      0.0839716, 2.18791e-16, 4.98587e-17, 0.0153284, 0.0839716, 6.65767e-20,
      9.1556e-18, 0.0342226, 0.13797, 0.0481524, -0.0481524, 0.0659188,
      0.150536, 0.0765563, -0.0765563, 0.209557, 0.0839716, 0.154623,
      2.24414e-16, 0, 0.0839716, 0.154623, 4.12093e-17, 0, -0.0328303,
      -0.179053, 0.170183, 0.170183, -0.0275385, -0.128629, 0.110528, 0.110528,
      0.133948, -6.65767e-20, -9.1556e-18, -0.0153284, -0.0839716, -2.18791e-16,
      -4.98587e-17, -0.0153284, -0.0839716, -0.0342226, -0.13797, 0.0481524,
      0.0481524, 0.0659188, -0.150536, 0.0765563, 0.0765563, 0.209557,
      -4.12093e-17, 0, -0.0839716, -0.154623, -2.24414e-16, 0, -0.0839716,
      -0.154623, 0.0328303, 0.179053, -0.170183, 0.170183, -0.0275385, 0.128629,
      -0.110528, 0.110528, 0.133948, 0.0153284, 0.0839716, 6.65767e-20,
      9.1556e-18, 0.0153284, 0.0839716, 2.18791e-16, 4.98587e-17, 0.0342226,
      0.13797, -0.0481524, 0.0481524, 0.0659188, 0.150536, -0.0765563,
      0.0765563, 0.209557, 0.0839716, 0.154623, 4.12093e-17, 0, 0.0839716,
      0.154623, 2.24414e-16, 0;

  Eigen::MatrixXd momentum1_ref = Eigen::MatrixXd::Zero(17, 17);
  momentum1_ref << 0, 0, 0, 1.12967, 0, 0, 0, 0.154782, 0, 0.0328303, 0.0342226,
      -0.0328303, -0.0342226, -0.0328303, -0.0342226, 0.0328303, 0.0342226, 0,
      0, 0, 0.539423, 0, 0, 0, 0.521834, 0, 0.179053, 0.13797, -0.179053,
      -0.13797, -0.179053, -0.13797, 0.179053, 0.13797, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0.170183, 0.0481524, -0.170183, -0.0481524, 0.170183, 0.0481524,
      -0.170183, -0.0481524, -1.12967, -0.539423, 0, 0, 0, -0.234095, 0, 0, 0,
      0.0275385, -0.0659188, 0.0275385, -0.0659188, 0.0275385, -0.0659188,
      0.0275385, -0.0659188, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.170183, 0.0481524,
      0.170183, 0.0481524, -0.170183, -0.0481524, -0.170183, -0.0481524, 0, 0,
      0, 0.234095, 0, 0, 0, 0.442557, 0, 0.128629, 0.150536, -0.128629,
      -0.150536, -0.128629, -0.150536, 0.128629, 0.150536, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0.110528, 0.0765563, -0.110528, -0.0765563, 0.110528, 0.0765563,
      -0.110528, -0.0765563, -0.154782, -0.521834, 0, 0, 0, -0.442557, 0, 0, 0,
      -0.133948, -0.209557, -0.133948, -0.209557, -0.133948, -0.209557,
      -0.133948, -0.209557, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.110528, 0.0765563,
      0.110528, 0.0765563, -0.110528, -0.0765563, -0.110528, -0.0765563,
      -0.0328303, -0.179053, -0.170183, -0.0275385, -0.170183, -0.128629,
      -0.110528, 0.133948, -0.110528, -2.18791e-16, -4.98587e-17, -0.0153284,
      -0.0839716, -0.0153284, -0.0839716, -6.65767e-20, -9.1556e-18, -0.0342226,
      -0.13797, -0.0481524, 0.0659188, -0.0481524, -0.150536, -0.0765563,
      0.209557, -0.0765563, -2.24414e-16, 0, -0.0839716, -0.154623, -0.0839716,
      -0.154623, -4.12093e-17, 0, 0.0328303, 0.179053, 0.170183, -0.0275385,
      -0.170183, 0.128629, 0.110528, 0.133948, -0.110528, 0.0153284, 0.0839716,
      2.18791e-16, 4.98587e-17, 6.65767e-20, 9.1556e-18, 0.0153284, 0.0839716,
      0.0342226, 0.13797, 0.0481524, 0.0659188, -0.0481524, 0.150536, 0.0765563,
      0.209557, -0.0765563, 0.0839716, 0.154623, 2.24414e-16, 0, 4.12093e-17, 0,
      0.0839716, 0.154623, 0.0328303, 0.179053, -0.170183, -0.0275385, 0.170183,
      0.128629, -0.110528, 0.133948, 0.110528, 0.0153284, 0.0839716,
      6.65767e-20, 9.1556e-18, 2.18791e-16, 4.98587e-17, 0.0153284, 0.0839716,
      0.0342226, 0.13797, -0.0481524, 0.0659188, 0.0481524, 0.150536,
      -0.0765563, 0.209557, 0.0765563, 0.0839716, 0.154623, 4.12093e-17, 0,
      2.24414e-16, 0, 0.0839716, 0.154623, -0.0328303, -0.179053, 0.170183,
      -0.0275385, 0.170183, -0.128629, 0.110528, 0.133948, 0.110528,
      -6.65767e-20, -9.1556e-18, -0.0153284, -0.0839716, -0.0153284, -0.0839716,
      -2.18791e-16, -4.98587e-17, -0.0342226, -0.13797, 0.0481524, 0.0659188,
      0.0481524, -0.150536, 0.0765563, 0.209557, 0.0765563, -4.12093e-17, 0,
      -0.0839716, -0.154623, -0.0839716, -0.154623, -2.24414e-16, 0;

  Eigen::MatrixXd momentum2_ref = Eigen::MatrixXd::Zero(17, 17);

  momentum2_ref << 0, 0, 1.12967, 0, 0, 0, 0.154782, 0, 0, 0.0328303, 0.0342226,
      0.0328303, 0.0342226, -0.0328303, -0.0342226, -0.0328303, -0.0342226, 0,
      0, 0.539423, 0, 0, 0, 0.521834, 0, 0, 0.179053, 0.13797, 0.179053,
      0.13797, -0.179053, -0.13797, -0.179053, -0.13797, -1.12967, -0.539423, 0,
      0, 0, -0.234095, 0, 0, 0, 0.0275385, -0.0659188, 0.0275385, -0.0659188,
      0.0275385, -0.0659188, 0.0275385, -0.0659188, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.170183, 0.0481524, -0.170183, -0.0481524, 0.170183, 0.0481524,
      -0.170183, -0.0481524, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.170183, 0.0481524,
      -0.170183, -0.0481524, -0.170183, -0.0481524, 0.170183, 0.0481524, 0, 0,
      0.234095, 0, 0, 0, 0.442557, 0, 0, 0.128629, 0.150536, 0.128629, 0.150536,
      -0.128629, -0.150536, -0.128629, -0.150536, -0.154782, -0.521834, 0, 0, 0,
      -0.442557, 0, 0, 0, -0.133948, -0.209557, -0.133948, -0.209557, -0.133948,
      -0.209557, -0.133948, -0.209557, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.110528,
      0.0765563, -0.110528, -0.0765563, 0.110528, 0.0765563, -0.110528,
      -0.0765563, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.110528, 0.0765563, -0.110528,
      -0.0765563, -0.110528, -0.0765563, 0.110528, 0.0765563, -0.0328303,
      -0.179053, -0.0275385, -0.170183, -0.170183, -0.128629, 0.133948,
      -0.110528, -0.110528, -2.18791e-16, -4.98587e-17, -6.65767e-20,
      -9.1556e-18, -0.0153284, -0.0839716, -0.0153284, -0.0839716, -0.0342226,
      -0.13797, 0.0659188, -0.0481524, -0.0481524, -0.150536, 0.209557,
      -0.0765563, -0.0765563, -2.24414e-16, 0, -4.12093e-17, 0, -0.0839716,
      -0.154623, -0.0839716, -0.154623, -0.0328303, -0.179053, -0.0275385,
      0.170183, 0.170183, -0.128629, 0.133948, 0.110528, 0.110528, -6.65767e-20,
      -9.1556e-18, -2.18791e-16, -4.98587e-17, -0.0153284, -0.0839716,
      -0.0153284, -0.0839716, -0.0342226, -0.13797, 0.0659188, 0.0481524,
      0.0481524, -0.150536, 0.209557, 0.0765563, 0.0765563, -4.12093e-17, 0,
      -2.24414e-16, 0, -0.0839716, -0.154623, -0.0839716, -0.154623, 0.0328303,
      0.179053, -0.0275385, -0.170183, 0.170183, 0.128629, 0.133948, -0.110528,
      0.110528, 0.0153284, 0.0839716, 0.0153284, 0.0839716, 2.18791e-16,
      4.98587e-17, 6.65767e-20, 9.1556e-18, 0.0342226, 0.13797, 0.0659188,
      -0.0481524, 0.0481524, 0.150536, 0.209557, -0.0765563, 0.0765563,
      0.0839716, 0.154623, 0.0839716, 0.154623, 2.24414e-16, 0, 4.12093e-17, 0,
      0.0328303, 0.179053, -0.0275385, 0.170183, -0.170183, 0.128629, 0.133948,
      0.110528, -0.110528, 0.0153284, 0.0839716, 0.0153284, 0.0839716,
      6.65767e-20, 9.1556e-18, 2.18791e-16, 4.98587e-17, 0.0342226, 0.13797,
      0.0659188, 0.0481524, -0.0481524, 0.150536, 0.209557, 0.0765563,
      -0.0765563, 0.0839716, 0.154623, 0.0839716, 0.154623, 4.12093e-17, 0,
      2.24414e-16, 0;

  bool check_momentum0 = momentum.Matrix()[0].isApprox(momentum0_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_momentum0, 1);
  if (!check_momentum0) {
    cout << "ref" << endl;
    cout << momentum0_ref << endl;
    cout << "result" << endl;
    cout << momentum.Matrix()[0] << endl;
  }
  bool check_momentum1 = momentum.Matrix()[1].isApprox(momentum1_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_momentum1, 1);
  if (!check_momentum1) {
    cout << "ref" << endl;
    cout << momentum1_ref << endl;
    cout << "result" << endl;
    cout << momentum.Matrix()[1] << endl;
  }
  bool check_momentum2 = momentum.Matrix()[2].isApprox(momentum2_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_momentum2, 1);
  if (!check_momentum2) {
    cout << "ref" << endl;
    cout << momentum2_ref << endl;
    cout << "result" << endl;
    cout << momentum.Matrix()[2] << endl;
  }
}

BOOST_AUTO_TEST_CASE(large_l_test) {
  std::ofstream xyzfile("C2.xyz");
  xyzfile << " 2" << std::endl;
  xyzfile << " C2" << std::endl;
  xyzfile << " C            .000000     .000000     .000000" << std::endl;
  xyzfile << " C            1.000000     .000000     .000000" << std::endl;
  xyzfile.close();

  QMMolecule mol("C", 0);
  mol.LoadFromFile("C2.xyz");

  ofstream basisfile("G.xml");
  basisfile << "<basis name=\"G\">" << endl;
  basisfile << "  <element name=\"C\">" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"G\">" << endl;
  basisfile << "      <constant decay=\"5.447178e+00\">" << endl;
  basisfile << "        <contractions factor=\"1.562850e-01\" type=\"G\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "  </element>" << endl;
  basisfile << "</basis>" << std::endl;
  basisfile.close();

  BasisSet basisset;
  basisset.Load("G.xml");
  AOBasis dftbasis;
  dftbasis.Fill(basisset, mol);

  AODipole dip;
  dip.Fill(dftbasis);
  Index dftbasissize = 18;

  Eigen::MatrixXd dip0_ref = Eigen::MatrixXd::Zero(dftbasissize, dftbasissize);

  dip0_ref << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00698192, 0, 0, 0, -0.00896982, 0, 0,
      0, 0.00649193, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000964788, 0, 0, 0,
      -0.00240334, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0136987, 0, 0,
      0, 0.0111631, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00385377, 0, 0,
      0, 0.00744207, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00896982, 0, 0, 0,
      0.0122167, 0, 0, 0, -0.0112798, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00240334,
      0, 0, 0, 0.00641505, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0111631,
      0, 0, 0, -0.0108859, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00744207,
      0, 0, 0, -0.0207308, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00649193, 0, 0, 0,
      -0.0112798, 0, 0, 0, 0.0213368, 0.00698192, 0, 0, 0, -0.00896982, 0, 0, 0,
      0.00649193, 1.88973, 0, 0, 0, 3.46492e-17, 0, 0, 0, 1.94289e-16, 0,
      0.000964788, 0, 0, 0, -0.00240334, 0, 0, 0, 0, 1.88973, 0, 0, 0,
      1.92546e-16, 0, 0, 0, 0, 0, -0.0136987, 0, 0, 0, 0.0111631, 0, 0, 0, 0,
      1.88973, 0, 0, 0, -2.44805e-16, 0, 0, 0, 0, 0, -0.00385377, 0, 0, 0,
      0.00744207, 0, 0, 0, 0, 1.88973, 0, 0, 0, 1.2951e-17, 0, -0.00896982, 0,
      0, 0, 0.0122167, 0, 0, 0, -0.0112798, 1.48984e-16, 0, 0, 0, 1.88973, 0, 0,
      0, -1.36647e-16, 0, -0.00240334, 0, 0, 0, 0.00641505, 0, 0, 0, 0,
      1.44582e-16, 0, 0, 0, 1.88973, 0, 0, 0, 0, 0, 0.0111631, 0, 0, 0,
      -0.0108859, 0, 0, 0, 0, -2.15709e-16, 0, 0, 0, 1.88973, 0, 0, 0, 0, 0,
      0.00744207, 0, 0, 0, -0.0207308, 0, 0, 0, 0, 3.50989e-17, 0, 0, 0,
      1.88973, 0, 0.00649193, 0, 0, 0, -0.0112798, 0, 0, 0, 0.0213368,
      4.31659e-16, 0, 0, 0, -3.98495e-18, 0, 0, 0, 1.88973;

  Eigen::MatrixXd dip1_ref = Eigen::MatrixXd::Zero(dftbasissize, dftbasissize);
  dip1_ref << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00150191, 0, 0, 0,
      -0.00258254, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.000964788, 0, 0, 0,
      0.000386357, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000964788, 0, 0, 0,
      -0.00240334, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00150191, 0, 0, 0,
      0.00153947, 0, 0, 0, 0.000142264, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.00153947, 0, 0, 0, 0.0031273, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.00240334, 0, 0, 0, -0.00119144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.000386357, 0, 0, 0, 0.00119144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.00258254, 0, 0, 0, -0.0031273, 0, 0, 0, 0.00104397, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, -0.000142264, 0, 0, 0, -0.00104397, 0, 0, 0, 0,
      -0.00150191, 0, 0, 0, 0.00258254, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.000964788, 0, 0, 0, -0.000386357, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.000964788, 0, 0, 0, 0.00240334, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.00150191, 0, 0, 0, -0.00153947, 0, 0, 0, -0.000142264, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0.00153947, 0, 0, 0, -0.0031273, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, -0.00240334, 0, 0, 0, 0.00119144, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0.000386357, 0, 0, 0, -0.00119144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, -0.00258254, 0, 0, 0, 0.0031273, 0, 0, 0, -0.00104397, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0.000142264, 0, 0, 0, 0.00104397, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0;

  Eigen::MatrixXd dip2_ref = Eigen::MatrixXd::Zero(dftbasissize, dftbasissize);

  dip2_ref << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000135096, 0, 0, 0,
      0.000379317, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.000159912, 0, 0,
      0, 0.000986306, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.000135096, 0, 0, 0,
      0.000816107, 0, 0, 0, -0.00309704, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.000159912, 0, 0, 0, -0.000212013, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, -0.000816107, 0, 0, 0, 0.000133185, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0.000212013, 0, 0, 0, -0.0026893, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.000379317, 0, 0, 0, -0.000133185, 0, 0, 0, 0.00342749, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, -0.000986306, 0, 0, 0, 0.0026893, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0.00309704, 0, 0, 0, -0.00342749, 0, 0, 0, 0,
      -0.000135096, 0, 0, 0, -0.000379317, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0.000159912, 0, 0, 0, -0.000986306, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.000135096, 0, 0, 0, -0.000816107, 0, 0, 0, 0.00309704, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, -0.000159912, 0, 0, 0, 0.000212013, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0.000816107, 0, 0, 0, -0.000133185, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, -0.000212013, 0, 0, 0, 0.0026893, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0.000379317, 0, 0, 0, 0.000133185, 0, 0, 0, -0.00342749, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0.000986306, 0, 0, 0, -0.0026893, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, -0.00309704, 0, 0, 0, 0.00342749, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0;

  bool check_dip0 = dip.Matrix()[0].isApprox(dip0_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_dip0, 1);
  if (!check_dip0) {
    cout << "ref" << endl;
    cout << dip0_ref << endl;
    cout << "result" << endl;
    cout << dip.Matrix()[0] << endl;
  }
  bool check_dip1 = dip.Matrix()[1].isApprox(dip1_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_dip1, 1);
  if (!check_dip1) {
    cout << "ref" << endl;
    cout << dip1_ref << endl;
    cout << "result" << endl;
    cout << dip.Matrix()[1] << endl;
  }
  bool check_dip2 = dip.Matrix()[2].isApprox(dip2_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_dip2, 1);
  if (!check_dip2) {
    cout << "ref" << endl;
    cout << dip2_ref << endl;
    cout << "result" << endl;
    cout << dip.Matrix()[2] << endl;
  }

  AOMomentum momentum;
  momentum.Fill(dftbasis);

  Eigen::MatrixXd momentum0_ref =
      Eigen::MatrixXd::Zero(dftbasissize, dftbasissize);

  momentum0_ref << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0452282, 0, 0, 0, -0.0551799, 0,
      0, 0, 0.0280469, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00821239, 0, 0, 0,
      -0.0201019, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0955273, 0, 0, 0,
      0.07533, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0290925, 0, 0, 0,
      0.05022, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0551799, 0, 0, 0, 0.0727415, 0,
      0, 0, -0.0552111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0201019, 0, 0, 0,
      0.0537993, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.07533, 0, 0, 0,
      -0.076546, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.05022, 0, 0, 0,
      -0.142981, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0280469, 0, 0, 0, -0.0552111,
      0, 0, 0, 0.126837, -0.0452282, 0, 0, 0, 0.0551799, 0, 0, 0, -0.0280469, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00821239, 0, 0, 0, 0.0201019, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0955273, 0, 0, 0, -0.07533, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0.0290925, 0, 0, 0, -0.05022, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0.0551799, 0, 0, 0, -0.0727415, 0, 0, 0, 0.0552111, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0.0201019, 0, 0, 0, -0.0537993, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, -0.07533, 0, 0, 0, 0.076546, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, -0.05022, 0, 0, 0, 0.142981, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.0280469, 0, 0, 0, 0.0552111, 0, 0, 0, -0.126837, 0, 0, 0, 0, 0, 0, 0,
      0, 0;

  Eigen::MatrixXd momentum1_ref =
      Eigen::MatrixXd::Zero(dftbasissize, dftbasissize);
  momentum1_ref << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0100472, 0, 0, 0,
      -0.0145434, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00821239, 0, 0, 0,
      -0.01029, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00821239, 0, 0, 0,
      -0.0201019, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0100472, 0, 0, 0,
      -0.0180007, 0, 0, 0, 0.0293066, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.0180007, 0, 0, 0, 0.0336053, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.0201019, 0, 0, 0, 0.0290685, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.01029, 0, 0, 0, 0.0290685, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      -0.0145434, 0, 0, 0, 0.0336053, 0, 0, 0, -0.0942411, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0.0293066, 0, 0, 0, -0.0942411, 0, 0, 0, 0, -0.0100472, 0,
      0, 0, 0.0145434, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00821239, 0, 0, 0,
      0.01029, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.00821239, 0, 0, 0,
      0.0201019, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0100472, 0, 0, 0,
      0.0180007, 0, 0, 0, -0.0293066, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.0180007, 0, 0, 0, -0.0336053, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.0201019, 0, 0, 0, -0.0290685, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.01029, 0, 0, 0, -0.0290685, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0.0145434, 0, 0, 0, -0.0336053, 0, 0, 0, 0.0942411, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, -0.0293066, 0, 0, 0, 0.0942411, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0;

  Eigen::MatrixXd momentum2_ref =
      Eigen::MatrixXd::Zero(dftbasissize, dftbasissize);

  momentum2_ref << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0472832, 0, 0, 0,
      -0.0343107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00824288, 0, 0, 0,
      -0.0107452, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0472832, 0, 0, 0, -0.0583714,
      0, 0, 0, 0.0337403, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00824288, 0, 0, 0,
      -0.0195091, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0583714, 0, 0, 0,
      0.0464027, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0195091, 0, 0, 0,
      0.0292982, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0343107, 0, 0, 0, 0.0464027,
      0, 0, 0, -0.0373403, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0107452, 0, 0, 0,
      0.0292982, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0337403, 0, 0, 0,
      -0.0373403, 0, 0, 0, 0, -0.0472832, 0, 0, 0, 0.0343107, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, -0.00824288, 0, 0, 0, 0.0107452, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, -0.0472832, 0, 0, 0, 0.0583714, 0, 0, 0, -0.0337403, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, -0.00824288, 0, 0, 0, 0.0195091, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0.0583714, 0, 0, 0, -0.0464027, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0.0195091, 0, 0, 0, -0.0292982, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0.0343107, 0, 0, 0, -0.0464027, 0, 0, 0, 0.0373403, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0.0107452, 0, 0, 0, -0.0292982, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, -0.0337403, 0, 0, 0, 0.0373403, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0;

  bool check_momentum0 = momentum.Matrix()[0].isApprox(momentum0_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_momentum0, 1);
  if (!check_momentum0) {
    cout << "ref" << endl;
    cout << momentum0_ref << endl;
    cout << "result" << endl;
    cout << momentum.Matrix()[0] << endl;
  }
  bool check_momentum1 = momentum.Matrix()[1].isApprox(momentum1_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_momentum1, 1);
  if (!check_momentum1) {
    cout << "ref" << endl;
    cout << momentum1_ref << endl;
    cout << "result" << endl;
    cout << momentum.Matrix()[1] << endl;
  }
  bool check_momentum2 = momentum.Matrix()[2].isApprox(momentum2_ref, 0.0001);
  BOOST_CHECK_EQUAL(check_momentum2, 1);
  if (!check_momentum2) {
    cout << "ref" << endl;
    cout << momentum2_ref << endl;
    cout << "result" << endl;
    cout << momentum.Matrix()[2] << endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()
