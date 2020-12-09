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

#define BOOST_TEST_MODULE dftcoupling_test

// Third party includes
#include <boost/test/unit_test.hpp>

// Local VOTCA includes
#include "votca/xtp/dftcoupling.h"
#include <libint2/initialize.h>
using namespace votca::xtp;
using namespace votca;

BOOST_AUTO_TEST_SUITE(dftcoupling_test)

Eigen::MatrixXd ReadMatrixFromString(const std::string& matrix) {
  votca::tools::Tokenizer lines(matrix, "\n");

  std::vector<double> entries;
  Index cols = 0;
  Index rows = 0;
  for (auto line : lines) {
    if (line[0] == '#') {
      continue;
    }
    votca::tools::Tokenizer entries_tok(line, " ");
    std::vector<std::string> temp = entries_tok.ToVector();
    cols = Index(temp.size());
    rows++;
    for (const auto& s : temp) {
      entries.push_back(std::stod(s));
    }
  }

  return Eigen::Map<Eigen::MatrixXd>(entries.data(), rows, cols);
}

BOOST_AUTO_TEST_CASE(coupling) {
  libint2::initialize();
  std::ofstream opt("dftcoupling.xml");
  opt << "           <dftcoupling_options>" << std::endl;
  opt << "            <degeneracy>0.0</degeneracy>" << std::endl;
  opt << "            <levA>2</levA>" << std::endl;
  opt << "            <levB>2</levB>" << std::endl;
  opt << "        </dftcoupling_options>" << std::endl;
  opt.close();

  std::ofstream xyzfile("molecule.xyz");
  xyzfile << " 5" << std::endl;
  xyzfile << " methane" << std::endl;
  xyzfile << " C            .000000     .000000     .000000" << std::endl;
  xyzfile << " H            .629118     .629118     .629118" << std::endl;
  xyzfile << " H           -.629118    -.629118     .629118" << std::endl;
  xyzfile << " H            .629118    -.629118    -.629118" << std::endl;
  xyzfile << " H           -.629118     .629118    -.629118" << std::endl;
  xyzfile.close();

  std::ofstream basisfile("3-21G.xml");
  basisfile << "<basis name=\"3-21G\">" << std::endl;
  basisfile << "  <element name=\"H\">" << std::endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << std::endl;
  basisfile << "      <constant decay=\"5.447178e+00\">" << std::endl;
  basisfile << "        <contractions factor=\"1.562850e-01\" type=\"S\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "      <constant decay=\"8.245470e-01\">" << std::endl;
  basisfile << "        <contractions factor=\"9.046910e-01\" type=\"S\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "    </shell>" << std::endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << std::endl;
  basisfile << "      <constant decay=\"1.831920e-01\">" << std::endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"S\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "    </shell>" << std::endl;
  basisfile << "  </element>" << std::endl;
  basisfile << "  <element name=\"C\">" << std::endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << std::endl;
  basisfile << "      <constant decay=\"1.722560e+02\">" << std::endl;
  basisfile << "        <contractions factor=\"6.176690e-02\" type=\"S\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "      <constant decay=\"2.591090e+01\">" << std::endl;
  basisfile << "        <contractions factor=\"3.587940e-01\" type=\"S\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "      <constant decay=\"5.533350e+00\">" << std::endl;
  basisfile << "        <contractions factor=\"7.007130e-01\" type=\"S\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "    </shell>" << std::endl;
  basisfile << "    <shell scale=\"1.0\" type=\"SP\">" << std::endl;
  basisfile << "      <constant decay=\"3.664980e+00\">" << std::endl;
  basisfile << "        <contractions factor=\"-3.958970e-01\" type=\"S\"/>"
            << std::endl;
  basisfile << "        <contractions factor=\"2.364600e-01\" type=\"P\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "      <constant decay=\"7.705450e-01\">" << std::endl;
  basisfile << "        <contractions factor=\"1.215840e+00\" type=\"S\"/>"
            << std::endl;
  basisfile << "        <contractions factor=\"8.606190e-01\" type=\"P\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "    </shell>" << std::endl;
  basisfile << "    <shell scale=\"1.0\" type=\"SP\">" << std::endl;
  basisfile << "      <constant decay=\"1.958570e-01\">" << std::endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"S\"/>"
            << std::endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"P\"/>"
            << std::endl;
  basisfile << "      </constant>" << std::endl;
  basisfile << "    </shell> " << std::endl;
  basisfile << "  </element>" << std::endl;
  basisfile << "</basis>" << std::endl;
  basisfile.close();

  Orbitals A;
  A.setDFTbasisName("3-21G.xml");
  A.QMAtoms().LoadFromFile("molecule.xyz");
  A.setBasisSetSize(17);
  A.setNumberOfAlphaElectrons(5);
  A.setNumberOfOccupiedLevels(5);
  A.MOs().eigenvalues() = Eigen::VectorXd::Zero(17);
  A.MOs().eigenvalues() << -19.8117, -6.22408, -6.14094, -6.14094, -6.14094,
      -3.72889, -3.72889, -3.72889, -3.64731, -3.09048, -3.09048, -3.09048,
      -2.63214, -2.08206, -2.08206, -2.08206, -2.03268;

  A.MOs().eigenvectors() = Eigen::MatrixXd::Zero(17, 17);
  A.MOs().eigenvectors() << -0.996559, -0.223082, 4.81443e-15, 2.21045e-15,
      -6.16146e-17, -3.16966e-16, 5.46703e-18, -1.09681e-15, -0.0301914,
      6.45993e-16, 1.05377e-16, 3.41154e-16, -0.102052, -5.79826e-16,
      9.38593e-16, -4.69346e-15, -0.111923, -0.0445146, 0.88316, -1.94941e-14,
      -8.67388e-15, -7.26679e-16, 1.16326e-14, -3.35886e-15, 2.37877e-14,
      0.866126, 3.2068e-15, 3.80914e-15, 3.24563e-15, -0.938329, -6.4404e-15,
      1.10811e-14, -5.5056e-14, -1.28767, 8.15798e-17, 2.30849e-14, 1.04169,
      0.117804, 0.0951759, 0.179467, 0.147031, 0.39183, -1.02927e-14, 0.32699,
      -0.223689, -0.130009, 1.0375e-15, -0.0940179, 0.126956, 0.0122904,
      1.41709e-15, 4.60157e-17, -7.1203e-15, 0.143338, -0.980459, -0.355251,
      0.41611, -0.10826, -0.149964, 2.41546e-16, 0.12214, -0.0512447, 0.39537,
      1.1054e-15, -0.0996828, -0.0636092, -0.105478, 5.10746e-15, -5.25872e-18,
      4.8424e-15, 0.0488925, 0.364515, -0.9863, 0.0447336, 0.417155, -0.177023,
      5.76117e-15, -0.228081, -0.348136, 0.0253377, -1.05286e-15, 0.079576,
      0.0703157, -0.117608, 5.31327e-15, 0.0472716, 0.235837, -3.58018e-15,
      -1.68354e-15, 2.3989e-15, -9.86879e-15, 4.52519e-15, -1.6106e-14,
      -0.599523, -1.31237e-14, -8.63443e-15, -8.61196e-15, 1.8333, 2.05275e-14,
      -3.9562e-14, 1.89874e-13, 4.24316, -2.74184e-16, -1.53939e-15, -0.162416,
      -0.0183675, -0.0148395, -0.151162, -0.123842, -0.330032, 1.10084e-15,
      -1.45092, 0.992556, 0.576875, -3.82954e-15, 0.604373, -0.816111,
      -0.0790061, -8.89474e-15, -2.24862e-16, 3.23655e-15, -0.0223487, 0.152869,
      0.0553894, -0.350483, 0.0911859, 0.126313, -5.48468e-15, -0.541961,
      0.227383, -1.75434, -3.89443e-15, 0.640788, 0.408897, 0.67804,
      -3.17156e-14, -2.81346e-17, -1.09423e-15, -0.00762313, -0.0568338,
      0.15378, -0.0376785, -0.351364, 0.149104, -4.94425e-15, 1.01204, 1.54475,
      -0.112429, 8.50653e-15, -0.511536, -0.452008, 0.756019, -3.3547e-14,
      -0.00106227, 0.0237672, 0.00345981, -0.00139675, -0.00349474, -0.597906,
      -0.425733, -0.0605479, -0.343823, 0.162103, -0.45692, 0.21318, -0.600309,
      0.310843, -0.36406, 0.574148, 0.0554949, -0.00928842, -0.0414346,
      0.0619999, -0.0250297, -0.0626259, 0.00227746, 0.00162164, 0.00023063,
      -0.0301047, 0.273177, -0.770004, 0.359253, 0.0095153, -0.8783, 1.02867,
      -1.62228, -1.24406, -0.00106227, 0.0237672, 0.00238182, 0.00205737,
      0.00402848, 0.262742, 0.151145, -0.671213, -0.343823, 0.317484, 0.12884,
      -0.40386, -0.600309, 0.201313, -0.327527, -0.641099, 0.0554949,
      -0.00928842, -0.0414346, 0.0426822, 0.0368682, 0.0721904, -0.0010008,
      -0.000575719, 0.00255669, -0.0301047, 0.535026, 0.217123, -0.680588,
      0.0095153, -0.568818, 0.925441, 1.81145, -1.24406, -0.00106227, 0.0237672,
      -0.00318563, 0.0034409, -0.00203628, 0.514364, -0.353326, 0.391148,
      -0.343823, -0.496623, -0.0536813, -0.176018, -0.600309, -0.744328,
      -0.01898, 0.0665156, 0.0554949, -0.00928842, -0.0414346, -0.0570866,
      0.0616609, -0.0364902, -0.00195924, 0.00134584, -0.0014899, -0.0301047,
      -0.836913, -0.0904642, -0.296627, 0.0095153, 2.10313, 0.0536287,
      -0.187943, -1.24406, -0.00106227, 0.0237672, -0.002656, -0.00410152,
      0.00150255, -0.1792, 0.627913, 0.340613, -0.343823, 0.0170366, 0.38176,
      0.366698, -0.600309, 0.232172, 0.710567, 0.000435528, 0.0554949,
      -0.00928842, -0.0414346, -0.0475955, -0.0734994, 0.0269257, 0.000682583,
      -0.00239176, -0.00129742, -0.0301047, 0.0287103, 0.643346, 0.617962,
      0.0095153, -0.656011, -2.00774, -0.0012306, -1.24406;

  Orbitals B = A;
  B.QMAtoms().Translate(4 * Eigen::Vector3d::UnitX());

  Orbitals AB;
  AB.QMAtoms() = A.QMAtoms();
  AB.QMAtoms().AddContainer(B.QMAtoms());
  AB.MOs().eigenvalues().resize(34);
  AB.MOs().eigenvalues() << -10.1341, -10.1337, -0.808607, -0.665103, -0.474928,
      -0.455857, -0.455857, -0.365971, -0.365971, -0.263259, 0.140444, 0.154745,
      0.168775, 0.168775, 0.223948, 0.231217, 0.26323, 0.26323, 0.713478,
      0.713478, 0.793559, 0.885998, 0.944915, 0.944915, 1.01169, 1.04977,
      1.04977, 1.08863, 1.10318, 1.17822, 1.18094, 1.18094, 1.69037, 1.91046;
  std::string mos_ref_string =
      " -0.697309 -0.697192 -0.138562 0.154639 0.0435428 1.73438e-12 "
      "2.13093e-13 -8.65189e-14 -3.43449e-14 0.0581478 -0.13454 -0.0376033 "
      "5.46382e-12 8.3231e-14 0.111183 -0.00798105 1.59258e-13 1.63619e-12 "
      "2.3882e-12 -1.87201e-14 0.00654486 0.048026 3.94367e-12 -5.11935e-14 "
      "-0.0789143 3.6718e-14 1.52055e-12 -0.0134363 -0.00206854 -0.0770242 "
      "4.59464e-11 -2.01607e-13 -0.00162496 -0.0367745\n"
      " -0.0783921 -0.0769772 0.116956 -0.142166 -0.0554301 -3.50499e-12 "
      "-1.86741e-13 5.36568e-14 5.11008e-13 -0.0529852 0.0398041 0.00322307 "
      "-1.70454e-12 -1.13104e-13 -0.0504198 0.0144859 7.16041e-14 3.37842e-12 "
      "1.55705e-11 -9.16012e-14 0.110589 0.00602313 3.50438e-12 1.49731e-13 "
      "-0.0786935 7.1578e-13 8.77919e-12 -0.0741183 0.0994097 0.480567 "
      "-2.86802e-10 8.58629e-13 -1.41349 -1.37617\n"
      "-1.31663e-14 5.79687e-15 -9.12567e-13 -1.21281e-12 -9.79314e-12 "
      "0.179808 -0.175214 0.202197 0.194016 7.90197e-12 2.10679e-12 "
      "1.26699e-11 0.188156 0.116443 -7.73171e-12 -1.56645e-11 -0.255098 "
      "0.153207 -0.530812 0.119984 5.20064e-11 2.27813e-11 -0.194538 -0.39235 "
      "-3.45222e-11 -0.667081 -0.369257 3.03131e-11 -1.41125e-11 2.6854e-10 "
      "0.426545 0.234686 -1.21861e-12 7.38641e-13\n"
      "-1.97257e-14 -8.71107e-16 -6.32194e-13 -1.70457e-12 -1.12352e-11 "
      "0.180436 0.174567 -0.202601 0.193595 7.92911e-12 2.22435e-12 "
      "1.26533e-11 0.188465 -0.115943 -8.11243e-12 -1.54494e-11 0.256652 "
      "0.150589 -0.531118 -0.118621 5.18854e-11 2.30201e-11 -0.184528 0.397156 "
      "-3.33442e-11 0.667869 -0.36783 3.10521e-11 -1.40613e-11 2.68602e-10 "
      "0.4268 -0.234223 -9.00545e-13 9.31133e-13\n"
      " -0.00115489 0.000164776 0.0578394 0.0454767 0.251996 1.19171e-11 "
      "6.58841e-13 -2.64913e-13 1.44576e-11 -0.294909 -0.00802487 0.199834 "
      "-2.06998e-11 -1.42604e-13 0.151363 -0.218658 2.00335e-13 -2.66865e-12 "
      "3.61889e-11 -7.66537e-14 0.620011 -0.624288 -1.35313e-11 6.45206e-13 "
      "-0.0930979 -6.0734e-13 3.55302e-11 0.476225 0.450576 -0.323109 "
      "2.04055e-10 -7.22344e-13 0.0223133 -0.1443\n"
      " 0.060534 0.0561821 0.415836 -0.487318 -0.114633 1.54029e-12 "
      "-5.10868e-13 2.51948e-14 -2.28281e-12 -0.287976 1.82438 0.51819 "
      "-7.75715e-11 -1.07587e-12 -1.6604 0.033244 -2.25681e-12 -2.86385e-11 "
      "-2.19985e-12 1.54692e-13 0.1436 -0.365953 -3.10497e-11 2.09328e-14 "
      "0.237819 -1.55317e-12 -9.07554e-12 0.0534822 -0.272317 -0.708544 "
      "4.23309e-10 -9.43029e-13 2.71501 2.80085\n"
      " 2.08366e-13 -1.03736e-14 -1.14485e-12 1.42553e-13 -1.42216e-11 "
      "0.125879 -0.173468 0.166021 0.168907 -3.38267e-12 -4.79722e-12 "
      "5.09993e-11 0.639391 0.580945 -2.31268e-11 -4.28666e-11 -0.844352 "
      "0.75214 0.849064 -0.215142 -1.07708e-10 -3.4986e-11 -0.256404 1.12602 "
      "5.1508e-11 1.26525 0.754312 -6.50804e-11 3.10304e-11 -4.78706e-10 "
      "-0.741441 -0.00363983 1.77776e-12 -4.23163e-12\n"
      " 2.11528e-13 -6.42308e-15 -9.3007e-13 -2.46589e-13 -1.55798e-11 "
      "0.126502 0.173014 -0.166372 0.16856 -3.42581e-12 -5.1542e-12 "
      "5.02221e-11 0.640932 -0.579245 -2.38678e-11 -4.16714e-11 0.852002 "
      "0.743462 0.849613 0.212963 -1.07194e-10 -3.59559e-11 -0.284869 -1.11916 "
      "4.93541e-11 -1.26686 0.751605 -6.64254e-11 3.05998e-11 -4.76759e-10 "
      "-0.741444 0.00283525 1.41929e-12 -4.41113e-12\n"
      " 0.00325132 0.00072711 0.0753503 0.00950181 0.228655 1.62908e-11 "
      "5.88682e-13 -2.68345e-13 3.87226e-12 -0.28074 0.144232 0.869449 "
      "-8.4971e-11 -8.8691e-13 0.415771 -1.00835 4.58046e-13 -4.59974e-11 "
      "-1.24163e-10 3.99175e-13 -1.58015 0.920689 2.69425e-11 -1.12376e-12 "
      "0.183046 5.98902e-13 -2.88527e-11 -0.380313 -0.581711 0.578926 "
      "-3.6625e-10 1.2043e-12 -0.0303405 0.329309\n"
      " -0.0011464 0.00141574 0.102123 -0.062761 0.0756644 0.175473 "
      "-0.000315324 -0.00015246 0.14636 -0.14431 -0.0206219 -0.0846146 "
      "-0.0777281 -0.000103235 0.0556423 0.0507764 -0.000268375 -0.0524635 "
      "-0.511888 0.000656729 0.208213 -0.0190293 0.352659 -0.00447108 "
      "-0.391351 -0.000339021 0.317159 -0.515189 -0.458517 -0.471794 -0.629635 "
      "-0.000341627 0.0265973 -0.362435\n"
      " -0.013307 -0.00957248 -0.00732072 -0.00310248 0.0344914 0.171006 "
      "-0.000307297 -0.000150334 0.144318 -0.179084 -0.712591 -0.830754 "
      "-0.806581 -0.00107126 0.596627 0.752064 -0.00677785 -1.32497 0.0988994 "
      "-0.000126883 -0.474187 -0.145716 0.519372 -0.0065847 0.348342 "
      "0.00135561 -1.2682 0.790399 0.710069 0.136107 1.34459 0.000729549 "
      "-0.543167 -0.623166\n"
      " 8.55501e-05 0.000986043 0.0536401 -0.105273 -0.128709 -0.000224935 "
      "-0.125172 0.182036 0.000189623 0.101776 -0.00109152 0.0317062 "
      "0.000173788 -0.130849 0.00264037 -0.0712429 -0.0375076 0.000191869 "
      "-6.64731e-06 -0.00518125 -0.201684 0.377977 -0.00805124 -0.635046 "
      "-0.544273 0.358244 0.000382938 0.322411 0.418971 -0.334379 0.00033114 "
      "-0.610306 -0.0454904 -0.194439\n"
      " -0.0091205 -0.0117179 0.00848738 -0.0268224 -0.085757 -0.000140587 "
      "-0.0782345 0.184258 0.000191937 0.16027 -0.556338 0.388959 0.00126037 "
      "-0.948964 0.908403 -0.807446 1.13292 -0.00579543 0.000121528 0.0947251 "
      "-0.306129 0.0971475 -0.00206653 -0.162999 0.298931 -1.1321 -0.00121013 "
      "-0.454898 -0.615171 0.499217 -0.000372134 0.685859 -0.622467 -0.343531\n"
      " -0.0011464 0.00141574 0.102123 -0.062761 0.0756644 -0.175473 "
      "0.000315324 0.00015246 -0.14636 -0.14431 -0.0206219 -0.0846146 "
      "0.0777281 0.000103235 0.0556423 0.0507764 0.000268375 0.0524635 "
      "0.511888 -0.000656729 0.208213 -0.0190293 -0.352659 0.00447108 "
      "-0.391351 0.000339021 -0.317159 -0.515189 -0.458517 -0.471794 0.629635 "
      "0.000341627 0.0265973 -0.362435\n"
      " -0.013307 -0.00957248 -0.00732072 -0.00310248 0.0344914 -0.171006 "
      "0.000307297 0.000150334 -0.144318 -0.179084 -0.712591 -0.830754 "
      "0.806581 0.00107126 0.596627 0.752064 0.00677785 1.32497 -0.0988994 "
      "0.000126883 -0.474187 -0.145716 -0.519372 0.0065847 0.348342 "
      "-0.00135561 1.2682 0.790399 0.710069 0.136107 -1.34459 -0.000729549 "
      "-0.543167 -0.623166\n"
      " 8.55501e-05 0.000986043 0.0536401 -0.105273 -0.128709 0.000224935 "
      "0.125172 -0.182036 -0.000189623 0.101776 -0.00109152 0.0317062 "
      "-0.000173788 0.130849 0.00264037 -0.0712429 0.0375076 -0.000191869 "
      "6.6473e-06 0.00518125 -0.201684 0.377977 0.00805124 0.635046 -0.544273 "
      "-0.358244 -0.000382937 0.322411 0.418971 -0.334379 -0.00033114 0.610306 "
      "-0.0454904 -0.194439\n"
      " -0.0091205 -0.0117179 0.00848738 -0.0268224 -0.085757 0.000140587 "
      "0.0782345 -0.184258 -0.000191937 0.16027 -0.556338 0.388959 -0.00126037 "
      "0.948964 0.908403 -0.807446 -1.13292 0.00579543 -0.000121528 -0.0947251 "
      "-0.306129 0.0971475 0.00206653 0.162999 0.298931 1.1321 0.00121013 "
      "-0.454898 -0.615171 0.499217 0.000372133 -0.685859 -0.622467 -0.343531\n"
      " -0.697309 0.697192 -0.138562 -0.154639 0.0435428 1.49203e-12 "
      "9.77213e-14 1.24841e-14 1.25141e-12 -0.0581478 -0.13454 0.0376033 "
      "-7.21201e-12 7.1008e-14 -0.111183 -0.00798105 -9.94282e-15 -6.00058e-13 "
      "2.88443e-13 1.04188e-14 -0.00654486 0.048026 -1.03258e-12 -3.53311e-14 "
      "0.0789143 -1.02089e-13 -7.69953e-12 0.0134363 -0.00206854 -0.0770242 "
      "4.60228e-11 -1.45926e-13 0.00162496 -0.0367745\n"
      " -0.0783921 0.0769772 0.116956 0.142166 -0.0554301 -5.20584e-12 "
      "-1.00581e-13 1.64705e-14 -3.0152e-12 0.0529852 0.0398041 -0.00322307 "
      "2.08288e-13 -1.90771e-14 0.0504198 0.0144859 3.37325e-14 -1.30319e-13 "
      "-2.38237e-11 1.44013e-13 -0.110589 0.00602313 -3.17329e-12 -1.06756e-14 "
      "0.0786935 1.78234e-13 2.00784e-11 0.0741183 0.0994097 0.480567 "
      "-2.8943e-10 1.1105e-12 1.41349 -1.37617\n"
      "-1.94715e-14 1.93153e-14 -1.63987e-12 1.9805e-12 -5.25726e-12 0.174567 "
      "-0.180436 -0.193595 -0.202601 -6.02172e-12 5.04867e-12 -1.17883e-12 "
      "-0.115943 -0.188465 2.56675e-12 -2.29827e-12 -0.150589 0.256652 "
      "0.118621 -0.531118 -1.87996e-12 -1.40338e-11 0.397156 0.184528 "
      "-3.11885e-11 -0.36783 -0.667869 3.25477e-11 1.46489e-11 -1.16403e-10 "
      "-0.234223 -0.4268 5.13688e-12 1.85941e-12\n"
      "-2.76824e-14 2.71818e-14 -1.50687e-12 2.10656e-12 -6.03834e-12 0.175214 "
      "0.179808 0.194016 -0.202197 -6.09358e-12 5.43735e-12 -9.64377e-13 "
      "-0.116443 0.188156 2.44188e-12 -2.23432e-12 0.153207 0.255098 0.119984 "
      "0.530812 -1.64208e-12 -1.40718e-11 0.39235 -0.194538 -3.09928e-11 "
      "0.369257 -0.667081 3.30204e-11 1.49297e-11 -1.18997e-10 -0.234686 "
      "0.426545 5.05575e-12 1.94355e-12\n"
      " 0.00115489 0.000164776 -0.0578394 0.0454767 -0.251996 -5.69911e-12 "
      "-7.97142e-13 5.68508e-14 1.3035e-11 -0.294909 0.00802487 0.199834 "
      "7.61611e-12 -9.8441e-14 0.151363 0.218658 -5.89857e-14 8.58175e-12 "
      "1.10677e-10 -3.79358e-13 0.620011 0.624288 -7.96112e-12 -1.08888e-13 "
      "-0.0930979 1.20145e-13 3.63482e-11 0.476225 -0.450576 0.323109 "
      "-2.09343e-10 7.74471e-13 0.0223133 0.1443\n"
      " 0.060534 -0.0561821 0.415836 0.487318 -0.114633 4.73838e-12 "
      "-2.00065e-13 -3.44901e-14 -4.54885e-12 0.287976 1.82438 -0.51819 "
      "1.02866e-10 -8.89747e-13 1.6604 0.033244 2.01243e-13 4.8244e-12 "
      "-5.98145e-12 -2.17907e-13 -0.1436 -0.365953 1.00174e-11 2.37019e-13 "
      "-0.237819 -2.93644e-13 -1.26674e-11 -0.0534822 -0.272317 -0.708544 "
      "4.1826e-10 -1.73009e-12 -2.71501 2.80085\n"
      " 2.66098e-13 -3.10494e-13 5.25454e-13 -3.01832e-13 -8.47312e-12 "
      "0.173014 -0.126502 -0.16856 -0.166372 -2.37598e-12 -2.19896e-12 "
      "-3.16763e-11 -0.579245 -0.640932 1.7174e-11 1.07442e-11 -0.743462 "
      "0.852002 -0.212963 0.849613 -8.91957e-12 3.46585e-11 -1.11916 0.284869 "
      "5.32058e-11 0.751605 1.26686 -6.82198e-11 -9.26745e-12 -4.60515e-11 "
      "0.00283525 0.741444 -9.92385e-12 -1.24539e-12\n"
      " 2.71095e-13 -3.13867e-13 6.16964e-13 -2.6292e-13 -9.09244e-12 0.173468 "
      "0.125879 0.168907 -0.166021 -2.49777e-12 -9.54976e-13 -3.07486e-11 "
      "-0.580945 0.639391 1.64204e-11 1.15619e-11 0.75214 0.844352 -0.215142 "
      "-0.849064 -9.09168e-12 3.41739e-11 -1.12602 -0.256404 5.25065e-11 "
      "-0.754312 1.26525 -6.90363e-11 -9.8764e-12 -4.12883e-11 0.00363983 "
      "-0.741441 -9.93678e-12 -1.37646e-12\n"
      " -0.00325132 0.00072711 -0.0753503 0.00950181 -0.228655 -1.83426e-11 "
      "-6.23026e-13 1.16744e-13 -1.52124e-12 -0.28074 -0.144232 0.869449 "
      "-8.0255e-12 -3.3796e-13 0.415771 1.00835 -2.11614e-13 8.62638e-13 "
      "-2.5409e-10 9.75851e-13 -1.58015 -0.920689 2.99441e-11 -5.07104e-14 "
      "0.183046 -4.50746e-13 -4.51192e-11 -0.380313 0.581711 -0.578926 "
      "3.56562e-10 -1.33803e-12 -0.0303405 -0.329309\n"
      " 8.55501e-05 -0.000986043 0.0536401 0.105273 -0.128709 0.125172 "
      "-0.000224935 0.000189623 -0.182036 -0.101776 -0.00109152 -0.0317062 "
      "0.130849 0.000173788 -0.00264037 -0.0712429 0.000191869 0.0375076 "
      "-0.00518125 6.64731e-06 0.201684 0.377977 0.635046 -0.00805124 0.544273 "
      "-0.000382938 0.358244 -0.322411 0.418971 -0.334379 0.610306 0.00033114 "
      "0.0454904 -0.194439\n"
      " -0.0091205 0.0117179 0.00848738 0.0268224 -0.085757 0.0782345 "
      "-0.000140587 0.000191937 -0.184258 -0.16027 -0.556338 -0.388959 "
      "0.948964 0.00126037 -0.908403 -0.807446 -0.00579543 -1.13292 0.0947251 "
      "-0.000121528 0.306129 0.0971475 0.162999 -0.00206653 -0.298931 "
      "0.00121013 -1.1321 0.454898 -0.615171 0.499217 -0.685859 -0.000372134 "
      "0.622467 -0.343531\n"
      " -0.0011464 -0.00141574 0.102123 0.062761 0.0756644 -0.000315324 "
      "-0.175473 -0.14636 -0.00015246 0.14431 -0.0206219 0.0846146 "
      "-0.000103235 0.0777281 -0.0556423 0.0507764 0.0524635 -0.000268375 "
      "-0.000656729 -0.511888 -0.208213 -0.0190293 -0.00447108 -0.352659 "
      "0.391351 0.317159 0.000339021 0.515189 -0.458517 -0.471794 -0.000341627 "
      "0.629635 -0.0265973 -0.362435\n"
      " -0.013307 0.00957248 -0.00732072 0.00310248 0.0344914 -0.000307297 "
      "-0.171006 -0.144318 -0.000150334 0.179084 -0.712591 0.830754 "
      "-0.00107126 0.806581 -0.596627 0.752064 1.32497 -0.00677785 0.000126883 "
      "0.0988994 0.474187 -0.145716 -0.0065847 -0.519372 -0.348342 -1.2682 "
      "-0.00135561 -0.790399 0.710069 0.136107 0.000729549 -1.34459 0.543167 "
      "-0.623166\n"
      " 8.55501e-05 -0.000986043 0.0536401 0.105273 -0.128709 -0.125172 "
      "0.000224935 -0.000189623 0.182036 -0.101776 -0.00109152 -0.0317062 "
      "-0.130849 -0.000173788 -0.00264037 -0.0712429 -0.000191869 -0.0375076 "
      "0.00518125 -6.64731e-06 0.201684 0.377977 -0.635046 0.00805124 0.544273 "
      "0.000382938 -0.358244 -0.322411 0.418971 -0.334379 -0.610306 "
      "-0.00033114 0.0454904 -0.194439\n"
      " -0.0091205 0.0117179 0.00848738 0.0268224 -0.085757 -0.0782345 "
      "0.000140587 -0.000191937 0.184258 -0.16027 -0.556338 -0.388959 "
      "-0.948964 -0.00126037 -0.908403 -0.807446 0.00579543 1.13292 -0.0947251 "
      "0.000121528 0.306129 0.0971475 -0.162999 0.00206653 -0.298931 "
      "-0.00121013 1.1321 0.454898 -0.615171 0.499217 0.685859 0.000372134 "
      "0.622467 -0.343531\n"
      " -0.0011464 -0.00141574 0.102123 0.062761 0.0756644 0.000315324 "
      "0.175473 0.14636 0.00015246 0.14431 -0.0206219 0.0846146 0.000103235 "
      "-0.0777281 -0.0556423 0.0507764 -0.0524635 0.000268375 0.000656729 "
      "0.511888 -0.208213 -0.0190293 0.00447108 0.352659 0.391351 -0.317159 "
      "-0.000339021 0.515189 -0.458517 -0.471794 0.000341628 -0.629635 "
      "-0.0265973 -0.362435\n"
      " -0.013307 0.00957248 -0.00732072 0.00310248 0.0344914 0.000307297 "
      "0.171006 0.144318 0.000150334 0.179084 -0.712591 0.830754 0.00107126 "
      "-0.806581 -0.596627 0.752064 -1.32497 0.00677785 -0.000126883 "
      "-0.0988994 0.474187 -0.145716 0.0065847 0.519372 -0.348342 1.2682 "
      "0.00135561 -0.790399 0.710069 0.136107 -0.000729549 1.34459 0.543167 "
      "-0.623166";
  AB.setBasisSetSize(34);
  AB.setDFTbasisName(A.getDFTbasisName());
  AB.MOs().eigenvectors() = ReadMatrixFromString(mos_ref_string);
  AB.setNumberOfAlphaElectrons(10);
  AB.setNumberOfOccupiedLevels(10);
  votca::tools::Property prop;
  prop.LoadFromXML("dftcoupling.xml");
  DFTcoupling dftcoup;
  Logger log;
  dftcoup.setLogger(&log);
  dftcoup.Initialize(prop.get("dftcoupling_options"));
  log.setCommonPreface("\n... ...");
  dftcoup.CalculateCouplings(A, B, AB);

  votca::tools::Property output;
  dftcoup.Addoutput(output, A, B);

  std::map<Index, std::map<Index, double> > hole_ref;

  hole_ref[3][3] = -1.031779;
  hole_ref[3][4] = -8.652046;
  hole_ref[4][3] = 3.806175;
  hole_ref[4][4] = -5.745457;
  std::map<Index, std::map<Index, double> > electron_ref;
  electron_ref[5][5] = 3.080669;
  electron_ref[5][6] = 2.445706;
  electron_ref[6][5] = -1.622705;
  electron_ref[6][6] = 6.692015;

  auto holedata = output.Select("hole.coupling");
  for (auto h : holedata) {
    Index levA = h->getAttribute<Index>("levelA");
    Index levB = h->getAttribute<Index>("levelB");
    double j = h->getAttribute<double>("j");
    BOOST_CHECK_CLOSE(j, hole_ref[levA][levB], 1e-5);
  }
  auto electrondata = output.Select("electron.coupling");
  for (auto h : holedata) {
    Index levA = h->getAttribute<Index>("levelA");
    Index levB = h->getAttribute<Index>("levelB");
    double j = h->getAttribute<double>("j");
    BOOST_CHECK_CLOSE(j, electron_ref[levA][levB], 1e-5);
  }

  libint2::finalize();
}

BOOST_AUTO_TEST_SUITE_END()
