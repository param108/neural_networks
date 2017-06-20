import json
import sys

from sklearn.neural_network import MLPClassifier

def getSizes(t):
	maxx = 0
	maxy = 0
	for data in t:
		for sample in t[data]:
			for coords in sample:
				if coords[0] > maxx:
					maxx = coords[0]
				if coords[1] > maxy:
					maxy = coords[1]
	maxx = maxx + 1
	maxy = maxy + 1
	return maxx,maxy

def test_xy(X,y):
	length = len(X[0])
	print("Size of sample:"+str(length))
	for sample in X:
		if (length != len(sample)):
			print("FAIL")
			return False

	if len(X) != len(y):
		print ("Result and input do not match")
		return False
	return True


def doNormalize(maxx, maxy, sample):
	sampledata=[]
	for i in range(maxy):
		for j in range(maxx):
			if [j,i] in sample:
				sampledata.append(1.0)
			else:
				sampledata.append(0.0)
	return sampledata
	
def normalizeData(t):
	maxx = 50;
	maxy = 50;
	finaldata=[]
	result=[]
	for data in t:
		sampledata = doNormalize(maxx, maxy, data[0])
		finaldata.append(sampledata)
		if data[1] == "a":
			result.append(0.0)
		else:
			result.append(1.0)
	return maxx, maxy, finaldata,result		
		
		
fp = open(sys.argv[1],"r")
train = json.load(fp)
lenx, leny, X,y = normalizeData(train)

if test_xy(X,y):
	clf = MLPClassifier(solver='lbfgs', alpha=1e-5, hidden_layer_sizes=(1000,2), shuffle=True,random_state=3, warm_start=True)
	for i in range(20): 
		clf.fit(X,y)
		print(i)
	fppredict = open(sys.argv[2],"r")
	predict = json.load(fppredict)
	predict_ds = []
	predict_ans = []
	for sampledata in predict:
		predict_ds.append(doNormalize(50, 50, sampledata[0]))
		predict_ans.append(sampledata[1])
	print("Predicted:"+str(clf.predict(predict_ds)))
	print("Expected :"+str(predict_ans))
