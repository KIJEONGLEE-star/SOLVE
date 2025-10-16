#define MAX_STUDENTS 20000
#define NUM_SUBJECTS 5

int system[21000][5];
int universityScores[21000][31];

typedef struct {
	int weights[5];
} University;

typedef struct {
	int N;
	int M;
	University universities[31];
}Data;

static Data globalData;

void init(int N, int M, int mWeights[][5])
{
	for(int i=0; i<21000; i++)
		for(int j=0; j<5; j++)
			system[i][j] = 0;
	for(int i=1; i<=30; i++)
		for(int j=0; j<5; j++)
			globalData.universities[i].weights[j] = 0;
	if(N<2 || N>1000 || M<2 || M>30) return;
	globalData.N=N;
	globalData.M=M;

	for(int i=1; i<=M; i++)
		for(int j=0; j<5; j++)
			globalData.universities[i].weights[j] = mWeights[i-1][j];
}

void add(int mID, int mScores[5])
{
	if (mID > MAX_STUDENTS) return;
	for (int j=0; j<NUM_SUBJECTS; j++) system[mID][j] = mScores[j];

	for(int u=1; u<=globalData.M; u++) {
		int totalScore = 0;
		for(int s=0; s<NUM_SUBJECTS; s++){
			totalScore += system[mID][s] * globalData.universities[u].weights[s];
		}
		universityScores[mID][u] = totalScore;
	}
}
void erase(int mID)
{
	if(mID > MAX_STUDENTS) return;
	for(int j=0; j<NUM_SUBJECTS; j++) system[mID][j] = 0;

	for(int u=1; u<=globalData.M; u++){
		universityScores[mID][u] = 0;
	}
}
int suggest(int mID)
{
	bool globalSelected[MAX_STUDENTS+1] = {false};
	int result[MAX_STUDENTS+1] = {0};

	for (int u=1; u<=globalData.M; u++){
		int selectedCount=0;

		while(selectedCount < globalData.N){
			int maxScore = -1;
			int selectedStudent = -1;

			for(int m=1; m<=MAX_STUDENTS; m++){
				if(!globalSelected[m] && universityScores[m][u] > maxScore) {
					maxScore = universityScores[m][u];
					selectedStudent = m;
				}
				else if (!globalSelected[m] && universityScores[m][u] == maxScore && m < selectedStudent) {
					selectedStudent = m;
				}
			}
			if(selectedStudent == -1) break;

			globalSelected[selectedStudent] = true;
			result[selectedStudent] = u;
			selectedCount++;
		}
	}
	if(result[mID] != 0) {
		return result[mID];
	}
	else{
		return -1;
	}
}




