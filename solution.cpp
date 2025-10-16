#define MAX_STUDENTS 20000
#define NUM_SUBJECTS 5

int studentScores[21000][5];
int universityScores[21000][31];

// Optimization: Track active students
int activeStudents[MAX_STUDENTS];
int activeCount = 0;
bool isActive[MAX_STUDENTS+1] = {false};

// Ultra-optimization: Cache simulation results
int cachedResults[MAX_STUDENTS+1] = {0};
bool cacheValid = false;


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
			studentScores[i][j] = 0;
	for(int i=1; i<=30; i++)
		for(int j=0; j<5; j++)
			globalData.universities[i].weights[j] = 0;
	
	// Reset active student tracking
	activeCount = 0;
	for(int i=0; i<=MAX_STUDENTS; i++) isActive[i] = false;
	cacheValid = false;
	
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
	
	// Add to active list if not already active
	if(!isActive[mID]) {
		activeStudents[activeCount++] = mID;
		isActive[mID] = true;
	}
	cacheValid = false;
	
	for (int j=0; j<NUM_SUBJECTS; j++) studentScores[mID][j] = mScores[j];

	for(int u=1; u<=globalData.M; u++) {
		int totalScore = 0;
		for(int s=0; s<NUM_SUBJECTS; s++){
			totalScore += studentScores[mID][s] * globalData.universities[u].weights[s];
		}
		universityScores[mID][u] = totalScore;
	}
}
void erase(int mID)
{
	if(mID > MAX_STUDENTS) return;
	
	// Remove from active list
	if(isActive[mID]) {
		isActive[mID] = false;
		// Find and remove from activeStudents array
		for(int i=0; i<activeCount; i++) {
			if(activeStudents[i] == mID) {
				activeStudents[i] = activeStudents[activeCount-1];
				activeCount--;
				break;
			}
		}
	}
	
	for(int j=0; j<NUM_SUBJECTS; j++) studentScores[mID][j] = 0;

	for(int u=1; u<=globalData.M; u++){
		universityScores[mID][u] = 0;
	}
	cacheValid = false;
}

int suggest(int mID)
{
	// Ultra-optimization: Use cached results if student state hasn't changed
	if(cacheValid) {
		return cachedResults[mID];
	}
	
	// Run simulation once and cache all results
	bool globalSelected[MAX_STUDENTS+1] = {false};
	for(int i=0; i<=MAX_STUDENTS; i++) cachedResults[i] = 0;

	for (int u=1; u<=globalData.M; u++){
		// Count remaining students for this university
		int remainingStudents = 0;
		int remainingList[MAX_STUDENTS];
		
		for(int i=0; i<activeCount; i++){
			int m = activeStudents[i];
			if(!globalSelected[m] && isActive[m]) {
				remainingList[remainingStudents++] = m;
			}
		}
		
		// Early selection optimization: if remaining <= N, select all without score calculations
		if(remainingStudents <= globalData.N) {
			for(int i=0; i<remainingStudents; i++) {
				int studentID = remainingList[i];
				globalSelected[studentID] = true;
				cachedResults[studentID] = u;
			}
		} else {
			// Traditional selection when we need to rank students
			for(int round = 0; round < globalData.N; round++) {
				int bestScore = -1;
				int bestStudent = -1;
				
				// Find the best remaining student for this university
				for(int i=0; i<activeCount; i++){
					int m = activeStudents[i];
					if(!globalSelected[m] && isActive[m]) {
						int currentScore = universityScores[m][u];
						bool isBetter = false;
						
						if(currentScore > bestScore) {
							isBetter = true;
						} else if(currentScore == bestScore && m < bestStudent) {
							isBetter = true;
						}
						
						if(isBetter) {
							bestScore = currentScore;
							bestStudent = m;
						}
					}
				}
				
				if(bestStudent == -1) break; // No more eligible students
				
				globalSelected[bestStudent] = true;
				cachedResults[bestStudent] = u;
			}
		}
	}
	
	// Mark cache as valid and return result
	cacheValid = true;
	return cachedResults[mID] != 0 ? cachedResults[mID] : -1;
}




