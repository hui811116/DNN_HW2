/***********************************************************************/
/*                                                                     */
/*   svm_struct_api.c                                                  */
/*                                                                     */
/*   Definition of API for attaching implementing SVM learning of      */
/*   structures (e.g. parsing, multi-label classification, HMM)        */ 
/*                                                                     */
/*   Author: Thorsten Joachims                                         */
/*   Date: 03.07.04                                                    */
/*                                                                     */
/*   Copyright (c) 2004  Thorsten Joachims - All rights reserved       */
/*                                                                     */
/*   This software is available for non-commercial use only. It must   */
/*   not be modified and distributed without prior permission of the   */
/*   author. The author is not responsible for implications from the   */
/*   use of this software.                                             */
/*                                                                     */
/***********************************************************************/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include "svm_struct_common.h"
#include "svm_struct_api.h"

#define	MAX_FEATURE_SIZE	1000
#define MAX_STATE_SIZE   	48
#define FEATURE_DIMENSION	69

// Helper function
void print(const double* vec, size_t vecSize){
  size_t i = 0;
  for(; i < vecSize; i++){
    printf("%f\n", vec[i]);
  }
}

double sumOfVec(const double* vec, size_t vecSize){
  double sum = 0.0;
  size_t i = 0;
  for(; i < vecSize; i++){
    sum += vec[i];
  }
  return sum;
}

void dotProduct(double* temp, const double* vec1, const double* vec2, size_t idx1, size_t idx2, size_t dotRange){
  size_t i = 0;
  for(; i < dotRange; i++){
    temp[i] = vec1[idx1+i] * vec2[idx2 + i];
  }
}


void        svm_struct_learn_api_init(int argc, char* argv[])
{
  /* Called in learning part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_learn_api_exit()
{
  /* Called in learning part at the very end to allow any clean-up
     that might be necessary. */
}

void        svm_struct_classify_api_init(int argc, char* argv[])
{
  /* Called in prediction part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_classify_api_exit()
{
  /* Called in prediction part at the very end to allow any clean-up
     that might be necessary. */
}

SAMPLE      read_struct_examples(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads struct examples and returns them in sample. The number of
     examples must be written into sample.n */
  SAMPLE   sample;  /* sample */
  EXAMPLE  *examples;
   /* replace by appropriate number of examples */
  long     n=3696;       /* number of examples */
  examples=(EXAMPLE *)my_malloc(sizeof(EXAMPLE)*n);
	FILE* fid;
	char name[80];
	int fnum=0;
	int unum=0;
	int i,j,boo=0;
	int lab;
	float storeFeature[1024*69];
	int storeLabel[1024];
	fid=fopen(file,"r");
	while(fscanf(fid,"%s",name)!=EOF){
		boo=fscanf(fid,"%d \n [",&fnum);
		for(i=0;i<fnum;++i){
			boo=fscanf(fid,"%d",&lab);
				storeLabel[i]=lab;
			for(j=0;j<68;++j)
					boo=fscanf(fid," %f",&(storeFeature[i*69+j]));
				boo=fscanf(fid," %f]\n[",&(storeFeature[i*69+68]));
		}
		examples[unum].x._pattern=(float *)malloc(69*fnum*sizeof(float));
		examples[unum].y._label=(int *)malloc(fnum*sizeof(int));
		examples[unum].x._fnum=fnum;
		examples[unum].x._dim=69;
		examples[unum].y._isEmpty=0;
		examples[unum].y._size=fnum;
		for(i=0;i<fnum;++i){
				examples[unum].y._label[i]=storeLabel[i];
			for(j=0;j<69;++j)
					examples[unum].x._pattern[i*69+j]=storeFeature[i*69+j];
		}
		unum++;
	}
	if(boo){printf("read_struct_examples done!\n");}
//  n=100; /* replace by appropriate number of examples */
  /* fill in your code here */
  sample.n=n;
  sample.examples=examples;
  return(sample);
}

void        init_struct_model(SAMPLE sample, STRUCTMODEL *sm, 
			      STRUCT_LEARN_PARM *sparm, LEARN_PARM *lparm, 
			      KERNEL_PARM *kparm)
{
  /* Initialize structmodel sm. The weight vector w does not need to be
     initialized, but you need to provide the maximum size of the
     feature space in sizePsi. This is the maximum number of different
     weights that can be learned. Later, the weight vector w will
     contain the learned weights for the model. */

  sm->sizePsi=100; /* replace by appropriate number of features */
}

CONSTSET    init_struct_constraints(SAMPLE sample, STRUCTMODEL *sm, 
				    STRUCT_LEARN_PARM *sparm)
{
  /* Initializes the optimization problem. Typically, you do not need
     to change this function, since you want to start with an empty
     set of constraints. However, if for example you have constraints
     that certain weights need to be positive, you might put that in
     here. The constraints are represented as lhs[i]*w >= rhs[i]. lhs
     is an array of feature vectors, rhs is an array of doubles. m is
     the number of constraints. The function returns the initial
     set of constraints. */
  CONSTSET c;
  long     sizePsi=sm->sizePsi;
  long     i;
  WORD     words[2];

  if(1) { /* normal case: start with empty set of constraints */
    c.lhs=NULL;
    c.rhs=NULL;
    c.m=0;
  }
  else { /* add constraints so that all learned weights are
            positive. WARNING: Currently, they are positive only up to
            precision epsilon set by -e. */
    c.lhs=my_malloc(sizeof(DOC *)*sizePsi);
    c.rhs=my_malloc(sizeof(double)*sizePsi);
    for(i=0; i<sizePsi; i++) {
      words[0].wnum=i+1;
      words[0].weight=1.0;
      words[1].wnum=0;
      /* the following slackid is a hack. we will run into problems,
         if we have move than 1000000 slack sets (ie examples) */
      c.lhs[i]=create_example(i,0,1000000+i,1,create_svector(words,"",1.0));
      c.rhs[i]=0.0;
    }
  }
  return(c);
}

LABEL       classify_struct_example(PATTERN x, STRUCTMODEL *sm, 
				    STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label yhat for pattern x that scores the highest
     according to the linear evaluation function in sm, especially the
     weights sm.w. The returned label is taken as the prediction of sm
     for the pattern x. The weights correspond to the features defined
     by psi() and range from index 1 to index sm->sizePsi. If the
     function cannot find a label, it shall return an empty label as
     recognized by the function empty_label(y). */
  LABEL y;

/* insert your code for computing the predicted label y here */
  y._label = (int*)malloc(sizeof(int)*x._fnum);
  //y.isEmpty = 0;
  y._size = x._fnum;

  double* pattern = x._pattern;
  size_t featureNum = x._fnum;
  size_t inputDim = x._dim;
  size_t stateNum = MAX_STATE_SIZE;

  double* weight = sm->w;
  size_t weightLength = sm->sizePsi;
  size_t transIdx = inputDim * stateNum;
  int* seq = y._label;

  assert( weightLength == stateNum * stateNum + inputDim * stateNum + 1);

  memset(seq, 0, featureNum*sizeof(int));
  double viterbiTemp[MAX_STATE_SIZE][MAX_FEATURE_SIZE];
  int viterbiTrack[MAX_STATE_SIZE][MAX_FEATURE_SIZE];

  memset(viterbiTemp, -DBL_MAX, sizeof(viterbiTemp));
  memset(viterbiTrack, -1, sizeof(viterbiTrack));

  double* phi = (double*)malloc(sizeof(double)*weightLength);
  double* temp = (double*)malloc(sizeof(double)*weightLength);
  
  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  for(k = 0; k < stateNum; k++){
	memset(phi, 0.0, weightLength*sizeof(double));
	//thrust::fill(phi.begin(), phi.end(), 0);
	memcpy(phi + k*inputDim, pattern, inputDim*sizeof(double));
	//thrust::copy(ptrX, ptrX + inputDim, phi.begin() + k*inputDim);
	phi[transIdx + k*stateNum + k] = 1;

	// printf("\n");
	// print(phi, weightLength);
	// printf("\n");

	dotProduct(temp, weight, phi, 0, 0, weightLength);
	//thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<double>());
	double sum = sumOfVec(temp, weightLength);
	//double sum = thrust::reduce(temp.begin(), temp.end(), (double) 0, thrust::plus<double>());
	viterbiTemp[k][0] = sum;
  }

  for(i = 1; i < featureNum-1; i++){
    for(k = 0; k < stateNum; k++){
	  for(j = 0; j < stateNum; j++){
	    memset(phi, 0, weightLength*sizeof(double));
		//thrust::fill(phi.begin(), phi.end(), 0);
		memcpy(phi + k*inputDim, pattern + i*inputDim, inputDim*sizeof(double));
		//thrust::copy(ptrX+i*inputDim, ptrX+(i+1)*inputDim, phi.begin() + k*inputDim);
		phi[transIdx + j*stateNum + k] = 1;

		dotProduct(temp, weight, phi, 0, 0, weightLength);
		//thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<double>());
		double sum = sumOfVec(temp, weightLength);
		//double sum = thrust::reduce(temp.begin(), temp.end(), (double) 0, thrust::plus<double>());
		if( viterbiTemp[k][i] < sum + viterbiTemp[j][i-1] ){
		  viterbiTemp[k][i] = sum + viterbiTemp[j][i-1];
		  viterbiTrack[k][i] = j;
		}
	  }
	}
  }

  for(k = 0; k < stateNum; k++){
    for(j = 0; j < stateNum; j++){
	  memset(phi, 0, weightLength*sizeof(double));
	  //thrust::fill(phi.begin(), phi.end(), 0);
	  memcpy(phi + k*inputDim, pattern + (featureNum-1)*inputDim, inputDim*sizeof(double));
	  //thrust::copy(ptrX + (featureNum-1)*inputDim, ptrX + featureNum*inputDim, phi.begin() + k*inputDim);
      phi[transIdx + stateNum*stateNum] = 1;
																				
	  dotProduct(temp, weight, phi, 0, 0, weightLength);
	  //thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<double>());
	  double sum = sumOfVec(temp, weightLength);
	  //double sum = thrust::reduce(temp.begin(), temp.end(), (double) 0, thrust::plus<double>());
	  if( viterbiTemp[k][featureNum-1] < sum + viterbiTemp[j][featureNum-2] ){
	    viterbiTemp[k][featureNum-1] = sum + viterbiTemp[j][featureNum-2];
		viterbiTrack[k][featureNum-1] = j;
	  }
	}
  }

  // Back tracking
  double maxValue = viterbiTemp[0][featureNum-1];
  size_t idx = 0;
  for(j = 0; j < stateNum; j++){
    if(maxValue < viterbiTemp[j][featureNum-1]){
	  maxValue = viterbiTemp[j][featureNum-1];
	  idx = j;
	}
  }
  seq[featureNum-1] = idx;
  for(i = featureNum-1; i > 0; i--){
    idx = viterbiTrack[idx][i];
	seq[i-1] = idx;
  }

  return(y);
}

LABEL       find_most_violated_constraint_slackrescaling(PATTERN x, LABEL y, 
						     STRUCTMODEL *sm, 
						     STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the slack rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)*(1-psi(x,y)+psi(x,ybar)) 

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL ybar;

  
  /* insert your code for computing the label ybar here */

  return(ybar);
}

LABEL       find_most_violated_constraint_marginrescaling(PATTERN x, LABEL y, 
						     STRUCTMODEL *sm, 
						     STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the margin rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)+psi(x,ybar)

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL ybar;

  /* insert your code for computing the label ybar here */
  ybar._label = (int*)malloc(sizeof(int)*x._fnum);
  //ybar.isEmpty = 0;
  ybar._size = x._fnum;
  
  double* pattern = x._pattern;
  size_t featureNum = x._fnum;
  size_t inputDim = x._dim;
  size_t stateNum = MAX_STATE_SIZE;

  double* weight = sm->w;
  size_t weightLength = sm->sizePsi;
  size_t transIdx = inputDim * stateNum;
  int* seq = ybar._label;

  assert( weightLength == stateNum * stateNum + inputDim * stateNum + 1);

  memset(seq, 0, featureNum*sizeof(int));
  double viterbiTemp[MAX_STATE_SIZE][MAX_FEATURE_SIZE];
  int viterbiTrack[MAX_STATE_SIZE][MAX_FEATURE_SIZE];

  memset(viterbiTemp, -DBL_MAX, sizeof(viterbiTemp));
  memset(viterbiTrack, -1, sizeof(viterbiTrack));

  double* phi = (double*)malloc(sizeof(double)*weightLength);
  double* temp = (double*)malloc(sizeof(double)*weightLength);
  
  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  for(k = 0; k < stateNum; k++){
	memset(phi, 0.0, weightLength*sizeof(double));
	//thrust::fill(phi.begin(), phi.end(), 0);
	memcpy(phi + k*inputDim, pattern, inputDim*sizeof(double));
	//thrust::copy(ptrX, ptrX + inputDim, phi.begin() + k*inputDim);
	phi[transIdx + k*stateNum + k] = 1;

	// printf("\n");
	// print(phi, weightLength);
	// printf("\n");

	dotProduct(temp, weight, phi, 0, 0, weightLength);
	//thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<double>());
	double sum = sumOfVec(temp, weightLength) + loss_viterbi(y, k, sparm, 0);
	//double sum = thrust::reduce(temp.begin(), temp.end(), (double) 0, thrust::plus<double>());
	viterbiTemp[k][0] = sum;
  }

  for(i = 1; i < featureNum-1; i++){
    for(k = 0; k < stateNum; k++){
	  for(j = 0; j < stateNum; j++){
	    memset(phi, 0, weightLength*sizeof(double));
		//thrust::fill(phi.begin(), phi.end(), 0);
		memcpy(phi + k*inputDim, pattern + i*inputDim, inputDim*sizeof(double));
		//thrust::copy(ptrX+i*inputDim, ptrX+(i+1)*inputDim, phi.begin() + k*inputDim);
		phi[transIdx + j*stateNum + k] = 1;

		dotProduct(temp, weight, phi, 0, 0, weightLength);
		//thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<double>());
		double sum = sumOfVec(temp, weightLength) + loss_viterbi(y, k, sparm, i);
		//double sum = thrust::reduce(temp.begin(), temp.end(), (double) 0, thrust::plus<double>());
		if( viterbiTemp[k][i] < sum + viterbiTemp[j][i-1] ){
		  viterbiTemp[k][i] = sum + viterbiTemp[j][i-1];
		  viterbiTrack[k][i] = j;
		}
	  }
	}
  }

  for(k = 0; k < stateNum; k++){
    for(j = 0; j < stateNum; j++){
	  memset(phi, 0, weightLength*sizeof(double));
	  //thrust::fill(phi.begin(), phi.end(), 0);
	  memcpy(phi + k*inputDim, pattern + (featureNum-1)*inputDim, inputDim*sizeof(double));
	  //thrust::copy(ptrX + (featureNum-1)*inputDim, ptrX + featureNum*inputDim, phi.begin() + k*inputDim);
      phi[transIdx + stateNum*stateNum] = 1;
																				
	  dotProduct(temp, weight, phi, 0, 0, weightLength);
	  //thrust::transform(phi.begin(), phi.end(), ptrW, temp.begin(), thrust::multiplies<double>());
	  double sum = sumOfVec(temp, weightLength) + loss_viterbi(y, k, sparm, featureNum-1);
	  //double sum = thrust::reduce(temp.begin(), temp.end(), (double) 0, thrust::plus<double>());
	  if( viterbiTemp[k][featureNum-1] < sum + viterbiTemp[j][featureNum-2] ){
	    viterbiTemp[k][featureNum-1] = sum + viterbiTemp[j][featureNum-2];
		viterbiTrack[k][featureNum-1] = j;
	  }
	}
  }

  // Back tracking
  double maxValue = viterbiTemp[0][featureNum-1];
  size_t idx = 0;
  for(j = 0; j < stateNum; j++){
    if(maxValue < viterbiTemp[j][featureNum-1]){
	  maxValue = viterbiTemp[j][featureNum-1];
	  idx = j;
	}
  }
  seq[featureNum-1] = idx;
  for(i = featureNum-1; i > 0; i--){
    idx = viterbiTrack[idx][i];
	seq[i-1] = idx;
  }


  return(ybar);
}

int         empty_label(LABEL y)
{
  /* Returns true, if y is an empty label. An empty label might be
     returned by find_most_violated_constraint_???(x, y, sm) if there
     is no incorrect label that can be found for x, or if it is unable
     to label x at all */  
	if ( y._isEmpty == 1 ) // 1 is true
		return 1;
	else 
		return 0;
}

SVECTOR     *psi(PATTERN x, LABEL y, STRUCTMODEL *sm,
		 STRUCT_LEARN_PARM *sparm)
{
  /* Returns a feature vector describing the match between pattern x
     and label y. The feature vector is returned as a list of
     SVECTOR's. Each SVECTOR is in a sparse representation of pairs
     <featurenumber:featurevalue>, where the last pair has
     featurenumber 0 as a terminator. Featurenumbers start with 1 and
     end with sizePsi. Featuresnumbers that are not specified default
     to value 0. As mentioned before, psi() actually returns a list of
     SVECTOR's. Each SVECTOR has a field 'factor' and 'next'. 'next'
     specifies the next element in the list, terminated by a NULL
     pointer. The list can be though of as a linear combination of
     vectors, where each vector is weighted by its 'factor'. This
     linear combination of feature vectors is multiplied with the
     learned (kernelized) weight vector to score label y for pattern
     x. Without kernels, there will be one weight in sm.w for each
     feature. Note that psi has to match
     find_most_violated_constraint_???(x, y, sm) and vice versa. In
     particular, find_most_violated_constraint_???(x, y, sm) finds
     that ybar!=y that maximizes psi(x,ybar,sm)*sm.w (where * is the
     inner vector product) and the appropriate function of the
     loss + margin/slack rescaling method. See that paper for details. */
    // SVECTOR *fvec=NULL;
	
  /* insert code for computing the feature vector for x and y here */
      assert(x._fnum==y._size);
    //  SVECTOR *fvec = (SVECTOR*)calloc(1, sizeof(SVECTOR));      
	SVECTOR *fvec = (SVECTOR*)malloc(sizeof(SVECTOR));
     // int labelSize = LABEL_MAX;
      size_t feature_vector_size = x._dim*LABEL_MAX+LABEL_MAX*LABEL_MAX;
      
      fvec->words = (WORD*)calloc(feature_vector_size+1,sizeof(WORD));
      int prevLabel = LABEL_MAX;
      size_t i,j;
      for( i=0;i<x._fnum;i++){
              for(j=0;j<x._dim;j++){
                      fvec->words[x._dim*y._label[i]+j].weight += x._pattern[i*x._dim+j];  

              }

              if(i>0) fvec->words[x._dim*LABEL_MAX+prevLabel*LABEL_MAX+y._label[i]].weight+=1;

      		prevLabel = y._label[i];
      }

      for( i=0;i<feature_vector_size;i++){
              fvec->words[i].wnum = i+1;
      }

	//for test
	//write_psi("TEST.txt",fvec);
		return(fvec);
}

double      loss(LABEL y, LABEL ybar, STRUCT_LEARN_PARM *sparm){
  /* loss for correct label y and predicted label ybar. The loss for
     y==ybar has to be zero. sparm->loss_function is set with the -l option. */
  if(sparm->loss_function == 0) { /* type 0 loss: 0/1 loss */
                                  /* return 0, if y==ybar. return 1 else */
      assert(y._size == ybar._size);
	  int i = 0;
	  for (i = 0; i < y._size; i++){
	      if (y._label[i] != ybar._label[i]){
		      return 1;
		  }
	  }
	  return 0; // all match
  }
  else {
    /* Put your code for different loss functions here. But then
       find_most_violated_constraint_???(x, y, sm) has to return the
       highest scoring label with the largest loss. */
		return 1; //TODO  return true loss instead of 1
  }
}
double      loss_viterbi(LABEL y, LABEL ybar, STRUCT_LEARN_PARM *sparm, int compSize){
  /* loss for correct label y and predicted label ybar. The loss for
     y==ybar has to be zero. sparm->loss_function is set with the -l option. */
  if(sparm->loss_function == 0) { /* type 0 loss: 0/1 loss */
                                  /* return 0, if y==ybar. return 1 else */
      assert(y._size >= compSize);
	  assert(ybar._size >= compSize);
	  int i = 0;
	  for (i = 0; i < compSize; i++){
	      if (y._label[i] != ybar._label[i]){
		      return 1;
		  }
	  }
	  return 0; // all match
  }
  else {
    /* Put your code for different loss functions here. But then
       find_most_violated_constraint_???(x, y, sm) has to return the
       highest scoring label with the largest loss. */
		return 1; //TODO  return true loss instead of 1
  }
}

int         finalize_iteration(double ceps, int cached_constraint,
			       SAMPLE sample, STRUCTMODEL *sm,
			       CONSTSET cset, double *alpha, 
			       STRUCT_LEARN_PARM *sparm)
{
  /* This function is called just before the end of each cutting plane iteration. ceps is the amount by which the most violated constraint found in the current iteration was violated. cached_constraint is true if the added constraint was constructed from the cache. If the return value is FALSE, then the algorithm is allowed to terminate. If it is TRUE, the algorithm will keep iterating even if the desired precision sparm->epsilon is already reached. */
  return(0);
}

void        print_struct_learning_stats(SAMPLE sample, STRUCTMODEL *sm,
					CONSTSET cset, double *alpha, 
					STRUCT_LEARN_PARM *sparm)
{
  /* This function is called after training and allows final touches to
     the model sm. But primarly it allows computing and printing any
     kind of statistic (e.g. training error) you might want. */
}

void        print_struct_testing_stats(SAMPLE sample, STRUCTMODEL *sm,
				       STRUCT_LEARN_PARM *sparm, 
				       STRUCT_TEST_STATS *teststats)
{
  /* This function is called after making all test predictions in
     svm_struct_classify and allows computing and printing any kind of
     evaluation (e.g. precision/recall) you might want. You can use
     the function eval_prediction to accumulate the necessary
     statistics for each prediction. */
}

void        eval_prediction(long exnum, EXAMPLE ex, LABEL ypred, 
			    STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, 
			    STRUCT_TEST_STATS *teststats)
{
  /* This function allows you to accumlate statistic for how well the
     predicition matches the labeled example. It is called from
     svm_struct_classify. See also the function
     print_struct_testing_stats. */
  if(exnum == 0) { /* this is the first time the function is
		      called. So initialize the teststats */
  }
}

void        write_struct_model(char *file, STRUCTMODEL *sm, 
			       STRUCT_LEARN_PARM *sparm)
{
  /* Writes structural model sm to file file. */
	FILE* fp;
	fp = fopen(file, "w");
	// write struct_model
	int i = 0;
	
	fprintf(fp, "size of w: %ld\n", sm->sizePsi);
	fprintf(fp, "w: ");
	for (i = 0; i < sm->sizePsi; i++){
		fprintf(fp, "%.6f ", sm->w[i]);
	}
	fprintf(fp, "\n");
	fprintf(fp, "walpha: %f\n", sm->walpha);
	// structure model unknown
	// write struct_model_parameter
	fprintf(fp, "epsilon: %f\n", sparm->epsilon);	
	fprintf(fp, "newconstretrain: %f\n", sparm->newconstretrain);
	fprintf(fp, "ccache_size: %d\n", sparm->ccache_size);
	fprintf(fp, "batch_size: %f\n", sparm->batch_size);
	fprintf(fp, "C: %f\n", sparm->C);
	fprintf(fp, "slack_norm: %d\n", sparm->slack_norm);
	fprintf(fp, "loss_type: %d\n", sparm->loss_type);
	fprintf(fp, "loss_function: %d\n", sparm->loss_function);
	// write custom arguments

	fprintf(fp, "custom_argc: %d\n", sparm->custom_argc);
	fprintf(fp, "custom_argv: \n");
	int j = 0;
	for (i = 0; i < sparm->custom_argc; i++){
		for (j = 0; j < 300; j++){
			printf(fp, "%c", sparm->custom_argv[i][j]);
		}
		printf("\n");
	}

	fclose(fp);
}


STRUCTMODEL read_struct_model(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads structural model sm from file file. This function is used
     only in the prediction module, not in the learning module. */
	STRUCTMODEL mdl;
	FILE* fp;
	int i;	
	fp = fopen(file, "r");
	// read struct model
	fscanf(fp, "size of w: %ld\n", &mdl.sizePsi);
	fscanf(fp, "w: ");
	for (i = 0; i < mdl.sizePsi; i++){
		fscanf(fp, "%.6f ", &mdl.w[i]);
	}	
	fscanf(fp, "walpha: %lf\n", &mdl.walpha);
	// structure model unknown
	// write struct_model_parameter
	fscanf(fp, "epsilon: %lf\n", &(sparm->epsilon));	
	fscanf(fp, "newconstretrain: %lf\n", &(sparm->newconstretrain));
	fscanf(fp, "ccache_size: %d\n", &(sparm->ccache_size));
	fscanf(fp, "batch_size: %lf\n", &(sparm->batch_size));
	fscanf(fp, "C: %lf\n", &(sparm->C));
	fscanf(fp, "slack_norm: %d\n", &(sparm->slack_norm));
	fscanf(fp, "loss_type: %d\n", &(sparm->loss_type));
	fscanf(fp, "loss_function: %d\n", &(sparm->loss_function));
	// write custom arguments

	fscanf(fp, "custom_argc: %d\n", &(sparm->custom_argc));
	fscanf(fp, "custom_argv: \n");
	int j = 0;
	for (i = 0; i < sparm->custom_argc; i++){
		for(j = 0; j < 300; j++){
			fscanf(fp, "%c ", &(sparm->custom_argv[i][j]));
		}
		fscanf(fp, "\n");
	}
	fclose(fp);
	//if(i) {printf("read_struct_model done!\n");}
	return mdl;
}

void        write_label(FILE *fp, LABEL y){
	int i;
	if(fp==NULL)perror("ERROR: write_label failed!(error opening file!)\n");
	fprintf(fp,"<label> %d\n",y._size);
	for(i=0;i<y._size;++i){
		fprintf(fp," %d",y._label[i]);
	}
	fprintf(fp,"\n");
  /* Writes label y to file handle fp. */
} 

void        free_pattern(PATTERN x) {
  /* Frees the memory of x. */
	free(x._pattern);
}

void        free_label(LABEL y) {
  /* Frees the memory of y. */
  free(y._label);
}

void        free_struct_model(STRUCTMODEL sm) 
{
  /* Frees the memory of model. */
  /* if(sm.w) free(sm.w); */ /* this is free'd in free_model */
  if(sm.svm_model) free_model(sm.svm_model,1);
  /* add free calls for user defined data here */
}

void        free_struct_sample(SAMPLE s)
{
  /* Frees the memory of sample s. */
  int i;
  for(i=0;i<s.n;i++) { 
    free_pattern(s.examples[i].x);
    free_label(s.examples[i].y);
  }
  free(s.examples);
}

void        print_struct_help()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_learn. */
  printf("         --* string  -> custom parameters that can be adapted for struct\n");
  printf("                        learning. The * can be replaced by any character\n");
  printf("                        and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      case 'a': i++; /* strcpy(learn_parm->alphafile,argv[i]); */ break;
      case 'e': i++; /* sparm->epsilon=atof(sparm->custom_argv[i]); */ break;
      case 'k': i++; /* sparm->newconstretrain=atol(sparm->custom_argv[i]); */ break;
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}

void        print_struct_help_classify()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_classify. */
  printf("         --* string -> custom parameters that can be adapted for struct\n");
  printf("                       learning. The * can be replaced by any character\n");
  printf("                       and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters_classify(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- for the
     classification module */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      /* case 'x': i++; strcpy(xvalue,sparm->custom_argv[i]); break; */
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}

//For Test
void		write_psi(char* file,SVECTOR *psi)
{
	FILE* fp;
	fp  = fopen(file,"w");
	int i;
	for(i=0;i<5616;i++){

		fprintf(fp,"  %d  %f\n",psi->words[i].wnum ,psi->words[i].weight);

	}
	fclose(fp);
		
}
