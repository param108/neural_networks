import json
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
				sampledata.append(1)
			else:
				sampledata.append(0)
	return sampledata
	
def normalizeData(t):
	maxx,maxy = getSizes(t)
	finaldata=[]
	result=[]
	for data in t:
		for sample in t[data]:
			sampledata = doNormalize(maxx, maxy, sample)
			finaldata.append(sampledata)
			if data == "a":
				result.append(0)
			else:
				result.append(1)
	return maxx, maxy, finaldata,result		
		
		
fp = open("data.json","r")
train = json.load(fp)
lenx, leny, X,y = normalizeData(train)

if test_xy(X,y):
	clf = MLPClassifier(solver='lbfgs', alpha=1e-5, hidden_layer_sizes=(5,2), random_state=1)
	clf.fit(X,y)

	atrial1 = [[6,11],[7,10],[8,9],[9,8],[10,7],[11,6],[11,5],[12,4],[14,4],[15,6],[13,3],[15,5],[16,6],[16,7],[17,8],[18,9],[19,10],[20,11],[10,8],[12,8],[11,8],[13,8],[14,8],[15,8],[16,8]]
	btrial1 = [[11,3],[11,4],[11,5],[11,6],[11,7],[11,8],[11,9],[11,10],[11,11],[12,3],[13,3],[14,3],[15,3],[16,3],[18,4],[18,5],[18,6],[17,7],[16,7],[13,7],[14,7],[12,7],[15,7],[18,8],[19,9],[18,10],[17,11],[15,11],[14,11],[12,11],[31,16]]
	atrial2 = [[9,3],[9,4],[9,5],[9,6],[9,7],[9,8],[9,9],[9,10],[9,11],[10,3],[12,3],[11,3],[13,3],[15,3],[14,3],[15,4],[15,5],[15,6],[15,7],[14,7],[13,7],[12,7],[11,7],[10,7],[15,8],[15,9],[15,10],[15,11]]

	sampleA= doNormalize(lenx, leny, atrial1)
	sampleB= doNormalize(lenx, leny, btrial1)
	sampleA1= doNormalize(lenx, leny, atrial2)
	print("atrial1:"+str(clf.predict([sampleA, sampleB, sampleA1])))
