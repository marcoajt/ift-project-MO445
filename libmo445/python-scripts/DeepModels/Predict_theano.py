# This script load a model and predict images.

# Arguments

# 1 = CSV file ou directory containing images 
# 2 = Model
# 3 = file weights if pretrained or None, otherwise.

# Author: Daniel Osaku 


import os
os.environ["KERAS_BACKEND"] = "theano"
import keras.backend as K
image_data_format="channels_first"
K.set_image_data_format(image_data_format)
from utils import ReadDataset
import time
import sys
import numpy as np
import json
from keras.models import Model
from keras.optimizers import SGD
from sklearn.metrics import classification_report, cohen_kappa_score, confusion_matrix, accuracy_score
sys.path.insert(0, "./Models/")

models = {
"InceptionV3",
"Vgg16",
"Vgg19",
"Squeezenet",
"Densenet121",
"Densenet161",
"Densenet169",
"Caffenet",
"Googlenet",
"Alexnet"
}


if __name__ == '__main__':
    inicio = time.time()
    if len(sys.argv)!=4:
       print("Usage: python <csv file or directory> <Model Name> <weights path>")
       print("Available models:")
       print(" -----> Alexnet")
       print(" -> Densenet121")
       print(" -> Densenet161")
       print(" -> Densenet169")
       print(" ---> Googlenet")
       print(" -> InceptionV3")
       print(" --> Squeezenet")
       print(" -------> Vgg16")
       print(" -------> Vgg19")
    else:

       ## parameters
       src         = sys.argv[1]
       NN          = sys.argv[2]
       weights     = sys.argv[3]  

       if NN not in models:
          print("The --model command line argument should be a key in the `models` dictionary")
          print("Available models:")
          print(" -----> Alexnet")
          print(" -> Densenet121")
          print(" -> Densenet161")
          print(" -> Densenet169")
          print(" ---> Googlenet")
          print(" --> Squeezenet")
          print(" -------> Vgg16")
          print(" -------> Vgg19")
       else:
          print ("import "+NN)
          exec ("import "+NN)

          ### Loading setup
          arq = open("./setup.json", "r")
          setup = json.load(arq)
          arq.close()


          ## Loading model
          exec("model = "+NN+".load_model(input_shape = ("+str(setup['channel'])+", "+str(setup['img_rows'])+","+str(setup['img_cols'])+"), num_classes = "+str(setup['num_classes'])+")")


          ## Loading model weights
          model.load_weights(weights, by_name=True)

          sgd = SGD(lr=setup["lr"], decay=setup["decay"], momentum=0.9, nesterov=True)
          model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])


          X, Y = ReadDataset.load_tensorflow(setup['channel'], src, setup['num_classes'], setup['img_rows'])
          X = ReadDataset.theano_format(X)
          #Optional
          #X = ReadDataset.preprocessing_imagenet(X, backend='theano')

          #Make predictions
          predictions_valid = model.predict(X, batch_size=setup['batch_size'], verbose=2)

          if NN=="Googlenet":
              # Combine 3 set of outputs using averaging
              predictions_valid = sum(predictions_valid)/len(predictions_valid)
          Y_test = np.argmax(Y, axis=-1) # Convert one-hot to index
          Y_pred = np.argmax(predictions_valid, axis=-1)

          print("*****************Results***************************")
          if len(Y_test)>1:
            print("Accuracy(%)    = ", accuracy_score(Y_test, Y_pred)*100)
            print("Cohen Kappa(%) = ", cohen_kappa_score(Y_test, Y_pred)*100)
            print(classification_report(Y_test, Y_pred))
            print("*****  Confusion Matrix  *****")
            print(confusion_matrix(Y_test, Y_pred))
          else: 
            print("Classified as ", str(Y_pred[0]))
            print("True Label: ", str(Y_test[0]))
          fim = time.time()
          total = fim-inicio
          print( "Execution time - ", fim-inicio," sec")

