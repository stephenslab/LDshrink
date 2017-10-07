#include <LDshrink.h>
#include<algorithm>
#include <functional>

// [[Rcpp::depends(RcppEigen)]]

//[[Rcpp::export]]
Eigen::MatrixXd haplo_2_geno(const Eigen::MatrixXd haplo, bool snps_in_rows=true){
  const bool sir=snps_in_rows;
  if(sir){
    const size_t n=haplo.cols()/2;
    const size_t p=haplo.rows();
    Eigen::MatrixXd retmat(p,n);
    for(size_t i=0;i<n;i++){
      retmat.col(i)=haplo.block(0,i*2,p,2).rowwise().sum();
    }
    return(retmat);
  }else{
    const size_t n=haplo.rows()/2;
    const size_t p=haplo.cols();
    Eigen::MatrixXd retmat(p,n);
    for(size_t i=0; i<n; i++){
      retmat.col(i)=haplo.block(i*2,0,2,p).colwise().sum();
    }
    return(retmat);
  }
}


//[[Rcpp::export]]
Eigen::MatrixXi haplo_2_geno_i(const Eigen::MatrixXi haplo, bool snps_in_rows=true){
  const bool sir=snps_in_rows;
  if(sir){
    const size_t n=haplo.cols()/2;
    const size_t p=haplo.rows();
    Eigen::MatrixXi retmat(p,n);
    for(size_t i=0;i<n;i++){
      retmat.col(i)=haplo.block(0,i*2,p,2).rowwise().sum();
    }
    return(retmat);
  }else{
    const size_t n=haplo.rows()/2;
    const size_t p=haplo.cols();
    Eigen::MatrixXi retmat(p,n);
    for(size_t i=0; i<n; i++){
      retmat.col(i)=haplo.block(i*2,0,2,p).colwise().sum();
    }
    return(retmat);
  }
}






//[[Rcpp::export]]
double calc_nmsum(const double m){
  int msize=(2*(int)m-1);
  Eigen::ArrayXd tx(msize);
  tx.setLinSpaced(msize,1,(int)(2*m-1));
  return  (1/tx).sum();
}

//[[Rcpp::export]]
double calc_theta(const double m){
  double nmsum=calc_nmsum(m);
  return((1/nmsum)/(2*m+1/nmsum));
}



//[[Rcpp::export]]
Rcpp::DataFrame ld2df(const Matrix_external ldmat, Rcpp::StringVector rsid,const double r2cutoff=0.01){
  using namespace Rcpp;
  size_t p=ldmat.rows();
  if(p!=ldmat.cols()){
    Rcpp::stop("ldmat is not square!");
  }
  if(p!=rsid.size()){
    Rcpp::stop("rsid must be of length p!");
  }
  size_t dfsize = (p*(p-1))/2;
  std::vector<double>corv;
  corv.reserve(dfsize);
  std::vector<std::string> rowsnp;
  std::vector<std::string> colsnp;
  rowsnp.reserve(dfsize);
  colsnp.reserve(dfsize);
  
  
  // Eigen::ArrayXd corv(dfsize);
  // Rcpp::StringVector rowsnp(dfsize);
  // Rcpp::StringVector colsnp(dfsize);
  // Rcpp::Rcout<<"Generating DataFrame"<<std::endl;
  size_t k=0;
  for(int i=0; i<p;i++){
    for(int j=i+1; j<p;j++ ){
      double r2=ldmat.coeff(i,j)*ldmat.coeff(i,j);
      if(r2>r2cutoff){
        corv.push_back(r2);
        rowsnp.push_back(as<std::string>(rsid[i]));
        colsnp.push_back(as<std::string>(rsid[j]));
      }
    }
  }
  // Rcpp::Rcout<<"Returning DataFrame"<<std::endl;
  
  return(DataFrame::create(_["rowsnp"]=wrap(rowsnp),_["colsnp"]=wrap(colsnp),_["r2"]=wrap(corv),_["stringsAsFactors"]=false));
}





Eigen::MatrixXd calc_cov( c_Matrix_internal mat){
  auto centered = mat.rowwise()-mat.colwise().mean();
  return (((centered.adjoint()*centered)/double(mat.rows()-1)));  
}







//[[Rcpp::export(name="calc_cov")]]
Eigen::MatrixXd calc_cov_exp(Matrix_external mat){
  return(calc_cov(mat));
}


Eigen::ArrayXd calc_variance(c_Matrix_internal mat){
  int n=mat.rows();
  return(((mat.rowwise()-(mat.colwise().mean())).array().square().colwise().sum())/(n-1));
}

//[[Rcpp::export(name="calc_variance")]]
Eigen::ArrayXd calc_variance_exp(Matrix_external mat){
  return(calc_variance(mat));
}


void cov_2_cor(Matrix_internal covmat){
  Eigen::ArrayXd rowvar=1/covmat.diagonal().array().sqrt();
  covmat.array().colwise()*=rowvar;
  covmat.array().rowwise()*=rowvar.transpose();
  covmat.diagonal().setOnes();
}

//[[Rcpp::export(name="cov_2_cor")]]
Eigen::MatrixXd cov_2_cor_exp(Matrix_external covmat){
  Eigen::MatrixXd tcovmat=covmat;
  cov_2_cor(tcovmat);
  return(tcovmat);
  
}




Eigen::MatrixXd calcLD(const c_Matrix_internal hmata,const c_arrayxd_internal mapa,const double m=85, const double Ne=11490.672741, const double cutoff=1e-3){

  double dosage_max=hmata.maxCoeff();
  bool isGeno=dosage_max>1;
  if(isGeno){
    Rcpp::Rcerr<<"LD is being estimated from genotype instead of haplotype"<<std::endl;
  }
  
  double theta=calc_theta(m);
  Eigen::MatrixXd S= calc_cov(hmata);
  if(isGeno){
    S*=0.5;
  }
  int numSNP=S.rows();
  S.triangularView<Eigen::StrictlyLower>().setZero();
  std::vector<double> mapv;
  mapv.resize(mapa.size());
  Eigen::VectorXd::Map(&mapv[0],mapa.size())=mapa;
  
  if(!is_sorted(mapv.begin(),mapv.end(),std::less<double>())){
    Rcpp::stop("Recombination map must be non-decreasing\n");
  }
  double tj=0;
  double ti=0;
  double rho=0;
  double tshrinkage;
  
  for(int i=0; i<numSNP;i++){
    ti=mapa(i);
    for(int j=i+1; j<numSNP;j++){
      tj=mapa(j);
      rho = 4*Ne*(tj-ti)/100;
      rho=-rho/(2*m);
      tshrinkage=std::exp(rho);
      if(tshrinkage<cutoff){
        tshrinkage=0;
      }
      S(i,j)=tshrinkage*S(i,j);
    }
  }
  
  // S=S+S.triangularView<Eigen::StrictlyUpper>().transpose();
  
  S.triangularView<Eigen::StrictlyLower>()=S.transpose();
  Eigen::ArrayXd eye(numSNP);
  eye.setOnes();
  eye=eye.array()*(0.5*theta * (1-0.5*theta));
  
  Eigen::MatrixXd SigHat = ((1-theta)*(1-theta))*S.array();
  SigHat.diagonal() = SigHat.diagonal().array()+eye;
  //       % SigHat is derived from Li and Stephens model (2003)
  //         SigHat = (1-theta)^2 * S + 0.5*theta * (1-0.5*theta) * eye(numSNP);
  // SigHat = (1-theta)^2 * S + 0.5*theta * (1-0.5*theta) * eye(numSNP);
  cov_2_cor(SigHat);
  return(SigHat);
}


//[[Rcpp::export(name="calcLD")]]
Eigen::MatrixXd calcLD_exp(Matrix_external hmata,arrayxd_external mapa,const double m=85, const double Ne=11490.672741, const double cutoff=1e-3){
  return(calcLD(hmata,mapa,m,Ne,cutoff));
}



Eigen::SparseMatrix<double> sp_calcLD(c_Matrix_internal hmata,c_arrayxd_internal mapa,const double m, const double Ne, const double cutoff){
  
  // compute_shrinkage_cor(dist,S,hmata,theta,m,Ne,cutoff);
  
  Eigen::MatrixXd dist = calcLD(hmata,mapa,m,Ne,cutoff);
  // dist.triangularView<Eigen::Lower>()=dist.transpose();
  Eigen::SparseMatrix<double> retmat=dist.sparseView();
  return(retmat);
}

//[[Rcpp::export(name="sp_calcLD")]]
Eigen::SparseMatrix<double> sp_calcLD_exp(Matrix_external hmata,arrayxd_external mapa,const double m, const double Ne, const double cutoff){
  return(sp_calcLD(hmata,mapa,m,Ne,cutoff));
}



Eigen::SparseMatrix<double> sp_calcLD_symm(c_Matrix_internal hmata,c_arrayxd_internal mapa,const double m, const double Ne, const double cutoff){
  
  // compute_shrinkage_cor(dist,S,hmata,theta,m,Ne,cutoff);
  
  Eigen::MatrixXd dist = calcLD(hmata,mapa,m,Ne,cutoff);
  // dist.triangularView<Eigen::Lower>()=dist.transpose();
  Eigen::SparseMatrix<double> retmat=Eigen::MatrixXd(dist.triangularView<Eigen::Upper>()).sparseView();
  return(retmat);
}

//[[Rcpp::export(name="sp_calcLD_symm")]]
Eigen::SparseMatrix<double> sp_calcLD_symm_exp(Matrix_external hmata,arrayxd_external mapa,const double m, const double Ne, const double cutoff){
  return(sp_calcLD_symm(hmata,mapa,m,Ne,cutoff));
}
