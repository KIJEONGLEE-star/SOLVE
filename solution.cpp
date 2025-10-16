#define MAX_STUDENTS 20000
#define NUM_SUBJECTS 5

int studentScores[21000][5];
int universityScores[21000][31];

// Optimization: Track active students
int activeStudents[MAX_STUDENTS];
int activeCount = 0;
bool isActive[MAX_STUDENTS+1] = {false};

// Ultra-simple: Pre-computed assignments for O(1) lookup
int assignedUniversity[MAX_STUDENTS+1] = {0};
bool needsUpdate = true;



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
	for(int i=0; i<=MAX_STUDENTS; i++) assignedUniversity[i] = 0;
	needsUpdate = true;
	
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
	needsUpdate = true;
	
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
	needsUpdate = true;
}

void updateAssignments() {
	bool globalSelected[MAX_STUDENTS+1] = {false};
	for(int i=0; i<=MAX_STUDENTS; i++) assignedUniversity[i] = 0;

	for (int u=1; u<=globalData.M; u++){
		// Count remaining eligible students
		int remainingCount = 0;
		for(int i=0; i<activeCount; i++){
			if(!globalSelected[activeStudents[i]] && isActive[activeStudents[i]]) {
				remainingCount++;
			}
		}
		
		// If remaining <= N, select all immediately
		if(remainingCount <= globalData.N) {
			for(int i=0; i<activeCount; i++){
				int studentID = activeStudents[i];
				if(!globalSelected[studentID] && isActive[studentID]) {
					globalSelected[studentID] = true;
					assignedUniversity[studentID] = u;
				}
			}
		} else {
			// Find top N students
			for(int round = 0; round < globalData.N; round++) {
				int bestScore = -1;
				int bestStudent = -1;
				
				for(int i=0; i<activeCount; i++){
					int m = activeStudents[i];
					if(!globalSelected[m] && isActive[m]) {
						if(universityScores[m][u] > bestScore || 
						   (universityScores[m][u] == bestScore && m < bestStudent)) {
							bestScore = universityScores[m][u];
							bestStudent = m;
						}
					}
				}
				
				if(bestStudent == -1) break;
				globalSelected[bestStudent] = true;
				assignedUniversity[bestStudent] = u;
			}
		}
	}
	needsUpdate = false;
}

int suggest(int mID)
{
	if(needsUpdate) {
		updateAssignments();
	}
	return assignedUniversity[mID] != 0 ? assignedUniversity[mID] : -1;
}




