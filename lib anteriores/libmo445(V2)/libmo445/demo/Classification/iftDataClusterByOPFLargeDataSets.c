#include "ift.h"

int main(int argc, char *argv[]) 
{
  iftDataSet      *Z=NULL,*Z_temp=NULL;
  iftKnnGraph     *graph=NULL;
  iftImage        *img=NULL;
  timer           *t1=NULL,*t2=NULL;
  int kmax;


  if (argc != 6)
      iftError(
              "Usage: iftDataClusterByOPFLargeDataSets <dataset.zip> <nb_train_samples> <kmax(percent or value)> <propagation_type(0:root_label/1:cluster_majority)> <debug(0:NO/1:YES)--flag to output messages and generate images with tsne>",
              "main");

  iftRandomSeed(time(NULL));
//  iftRandomSeed(IFT_RANDOM_SEED);
 
  Z = iftReadOPFDataSet(argv[1]);
  char debug = atoi(argv[5]);
  iftSetStatus(Z,IFT_TRAIN);



  iftSampler* sampler = iftRandomSubsampling(Z->nsamples, 1, atoi(argv[2]));
  iftDataSetSampling(Z, sampler, 0);
  iftDestroySampler(&sampler);

  if (debug){
    printf("Dataset characteristics: (ntrain_samples,nsamples, nfeats, nclasses): %d,%d, %d, %d\n\n", Z->ntrainsamples,Z->nsamples,Z->nfeats,Z->nclasses);

    int *n_samples=iftAllocIntArray(Z->nclasses+1);
    for (int s=0;s<Z->nsamples;s++){
      n_samples[Z->sample[s].truelabel]++;
    }

    printf("Number of samples by class\n");
    for (int i=1;i<=Z->nclasses;i++)
      printf("%d ",n_samples[i]);
    printf("\n\n");
    iftFree(n_samples);

    printf("ids -> ");
    for (int s=0;s<50;s++){
      printf("%d ",Z->sample[s].id);
    }
    printf("\n\n");

   /* printf("feats \n");
    for (int s=0;s<10;s++){
      for (int f=0;f<Z->nfeats;f++)
        printf("%.3f ",Z->sample[s].feat[f]);
      printf("\n");
    }
    printf("\n\n");*/
  }

  if (atof(argv[3]) >= 1.0)
    kmax=atof(argv[3]);
  else
    kmax=iftMax(atof(argv[3])*Z->ntrainsamples,1);

  t1      = iftTic();
  graph = iftCreateKnnGraph(Z,kmax);
  iftUnsupTrain(graph,iftNormalizedCut);
//  iftFastUnsupTrain(graph,iftNormalizedCut);
  iftUnsupClassify(graph,Z);
  t2     = iftToc();

  if (debug){
    fprintf(stdout,"clustering in %f ms with %d groups\n\n",iftCompTime(t1,t2),Z->ngroups);

   /* Z_temp=iftCopyDataSet(Z,true);*/

    /* check if we need to reduce the dataset*/
    /*if (Z_temp->nsamples > 2000) {
      iftSampler *sampler = iftRandomSubsampling(Z_temp->nsamples, 1, 2000);
      iftDataSetSampling(Z_temp, sampler, 0);
      iftDestroySampler(&sampler);

      iftDataSet *Zsmall = iftExtractSamples(Z_temp, IFT_TRAIN);
      iftDestroyDataSet(&Z_temp);
      Z_temp = Zsmall;
    }

    if (Z->nfeats > 2) {
      iftDataSet* Z2d;
      Z2d = iftDimReductionByTSNE(Z_temp, 2, 40, 1000);
      iftDestroyDataSet(&Z_temp);
      Z_temp=Z2d;
    }

    img = iftDraw2DFeatureSpace(Z_temp,GROUP,0);
    iftWriteImageP6(img,"groups.ppm");
    iftDestroyImage(&img);

    img = iftDraw2DFeatureSpace(Z_temp,CLASS,0);
    iftWriteImageP6(img,"classes.ppm");
    iftDestroyImage(&img);

    img = iftDraw2DFeatureSpace(Z_temp,WEIGHT,0);
    iftWriteImageP6(img,"weight.ppm");
    iftDestroyImage(&img);*/

    int count_prototypes=0;
    for (int s=0;s<Z->nsamples;s++)
      if (Z->sample[s].status == IFT_PROTOTYPE)
        count_prototypes++;

    printf("The clustering founded %d prototypes\n\n",count_prototypes);
  }
  else{
    fprintf(stdout,"%.3f %d",iftCompTime(t1,t2),Z->ngroups );
  }

  if (Z->nclasses > 0){
    /*check the way of propagation label*/
    if (atoi(argv[4]))
      iftPropagateClusterTrueLabels3(Z);
    else
      iftPropagateClusterTrueLabels2(Z);

    iftSetStatus(Z,IFT_TEST);
    float acc=iftTruePositives(Z);

    if (debug){
      printf("The accuracy after propagation was %.3f\n\n",acc);

      int *labels_founded=iftAllocIntArray(Z->nclasses+1);
      for (int s=0;s<Z->nsamples;s++)
        labels_founded[Z->sample[s].label]=1;

      for (int l=1;l<Z->nclasses+1;l++)
        if (!labels_founded[l])
          printf("\nLabel %d not founded after propagation phase\n",l);

      iftFree(labels_founded);

    /*  if (Z_temp){
        for (int s=0;s<Z_temp->nsamples;s++)
          Z_temp->sample[s].label=Z->sample[Z_temp->sample[s].id].label;
        Z_temp->ngroups=Z->ngroups;
      }

      img = iftDraw2DFeatureSpace(Z_temp,LABEL,0);
      iftWriteImageP6(img,"labels.ppm");
      iftDestroyImage(&img);*/
    }
    else{
      printf(" %.3f\n",acc);
    }
  }

  iftDestroyKnnGraph(&graph);
  iftDestroyDataSet(&Z_temp);
  iftDestroyDataSet(&Z);

  return(0);
}
