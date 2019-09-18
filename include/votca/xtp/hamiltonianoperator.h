/*
 * Copyright 2009-2019 The VOTCA Development Team (http://www.votca.org)
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


#ifndef __VOTCA_HAMILTONIANOPERATOR_H
#define __VOTCA_HAMILTONIANOPERATOR_H
#include <votca/xtp/bse_operator.h>
#include <votca/xtp/eigen.h>
namespace votca {
namespace xtp {

template<typename MatrixReplacement>
class HamiltonianOperator;

}
}  // namespace votca
namespace Eigen {
namespace internal {
// MatrixReplacement looks-like a SparseMatrix, so let's inherits its traits:
template <typename MatrixReplacement>
struct traits<votca::xtp::HamiltonianOperator<MatrixReplacement>>
    : public Eigen::internal::traits<Eigen::MatrixXd> {};
}  // namespace internal
}  // namespace Eigen

namespace votca {
namespace xtp {
template <typename MatrixReplacement>
class HamiltonianOperator
    : public Eigen::EigenBase<HamiltonianOperator<MatrixReplacement>> {
 public:
  // Required typedefs, constants, and method:
  typedef double Scalar;
  typedef double RealScalar;
  typedef int StorageIndex;
  enum {
    ColsAtCompileTime = Eigen::Dynamic,
    MaxColsAtCompileTime = Eigen::Dynamic,
    IsRowMajor = false
  };

  HamiltonianOperator(const MatrixReplacement &R,
                      const MatrixReplacement &C)
      : _R(R), _C(C),_size(2*C.cols()) {}; 
    
  class InnerIterator {
   public:
    InnerIterator(const HamiltonianOperator& xpr, const Eigen::Index& id)
        : _xpr(xpr), _id(id){};

    InnerIterator& operator++() {
      _row++;
      return *this;
    }
    operator bool() const {
      return _row < _xpr._size;
    }  // DO not use the size method, it returns linear dimension*linear
       // dimension i.e _size^2
    double value() const { return _xpr(_row, _id); }
    Eigen::Index row() const { return _row; }
    Eigen::Index col() const { return _id; }
    Eigen::Index index() const { return row(); }

   private:
    const HamiltonianOperator& _xpr;
    const Eigen::Index _id;
    Eigen::Index _row = 0;
  };

  Eigen::Index rows() const { return this->_size; }
  Eigen::Index cols() const { return this->_size; }
  Eigen::Index outerSize() const { return this->_size; }

  template <typename Vtype>
  Eigen::Product<HamiltonianOperator, Vtype, Eigen::AliasFreeProduct>
      operator*(const Eigen::MatrixBase<Vtype>& x) const {
    return Eigen::Product<HamiltonianOperator, Vtype,
                          Eigen::AliasFreeProduct>(*this, x.derived());
  }

  //  get a row of the operator
  Eigen::RowVectorXd row(int index) const {
    int lsize = this->_size;
    int halfsize = lsize/2;
   
    Eigen::RowVectorXd row_out = Eigen::RowVectorXd::Zero(lsize);

    if (index < halfsize)
    {

      Eigen::RowVectorXd r = this->_R.row(index);
      Eigen::RowVectorXd c = this->_C.row(index);

      for (int j = 0; j < halfsize; j++) {
          row_out(j) = r(j);
          row_out(j+halfsize) = c(j);
      }
    } else {

      Eigen::RowVectorXd r = this->_R.row(index-halfsize);
      Eigen::RowVectorXd c = this->_C.row(index-halfsize);

      for (int j = 0; j < halfsize; j++) {
          row_out(j) = -c(j);
          row_out(j+halfsize) = -r(j);
      }
    }
    return row_out;
  }

  // get the full matrix if we have to
  Eigen::MatrixXd get_full_matrix() const {
    Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(_size, _size);
  
    for (int i = 0; i < _size; i++) {
      matrix.row(i) = this->row(i);
    }
    return matrix;
  }

 private:

  const MatrixReplacement & _C;
  const MatrixReplacement & _R;
  int _size;

};
}  // namespace xtp
}  // namespace votca

namespace Eigen {
namespace internal {

// template <typename MatrixReplacement>
// struct traits<votca::xtp::HamiltonianOperator<MatrixReplacement>>
//     : public Eigen::internal::traits<Eigen::MatrixXd> {};

// replacement of the mat*vect operation
template <typename Vtype, typename MatrixReplacement>
struct generic_product_impl<votca::xtp::HamiltonianOperator<MatrixReplacement>, Vtype, DenseShape,
                            DenseShape, GemvProduct>
    : generic_product_impl_base<
          votca::xtp::HamiltonianOperator<MatrixReplacement>, Vtype,
          generic_product_impl<votca::xtp::HamiltonianOperator<MatrixReplacement>, Vtype> >{

  typedef
      typename Product<votca::xtp::HamiltonianOperator<MatrixReplacement>, Vtype>::Scalar Scalar;

  template <typename Dest>
  static void scaleAndAddTo(Dest& dst, const votca::xtp::HamiltonianOperator<MatrixReplacement>& op,
                            const Vtype& v, const Scalar& alpha) {
    // returns dst = alpha * op * v
    // alpha must be 1 here
    assert(alpha == Scalar(1) && "scaling is not implemented");
    EIGEN_ONLY_USED_FOR_DEBUG(alpha);

// make the mat vect product
#pragma omp parallel for
    for (int i = 0; i < op.rows(); i++) {
      dst(i) = op.row(i) * v;
    }
  }
};

}
}

#endif  //__VOTCA_HAMILTONIANOPERATOR_H