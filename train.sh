RATE=0.002
BSIZE=256
MAXEPOCH=20000
MOMENTUM="0 0.3 0.6 0.9"
DECAYSET="1 0.9 0.81 0.72"
DECAY=1
INITMODEL=model/momentExpInit.mdl
MODELDIR=model/initexp
UNIRANGE="0.1 0.5 1 2";
# ====== new parameters
C=0.01
TRAIN=/home/hui/model/svm_fbank.ark
LABEL=/home/ahpan/DeepLearningHW2/DNN_HW2/Data/MLDS_HW1_RELEASE_v1/label/train.lab 
gdb --args ./svm_struct/svm_empty_learn.app -c {C} {TRAIN} {LABEL} 


