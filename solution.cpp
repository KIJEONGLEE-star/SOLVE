#define MAX_STUDENTS 20000
#define NUM_SUBJECTS 5

int studentScores[21000][5];
int universityScores[21000][31];

// Optimization: Track active students
int activeStudents[MAX_STUDENTS];
int activeCount = 0;
bool isActive[MAX_STUDENTS+1] = {false};

// Optimization: Student-score pairs for faster sorting
typedef struct {
	int studentID;
	int score;
} StudentScore;

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
}
// Custom quicksort implementation for StudentScore array
void swap(StudentScore* a, StudentScore* b) {
	StudentScore temp = *a;
	*a = *b;
	*b = temp;
}

int partition(StudentScore arr[], int low, int high) {
	StudentScore pivot = arr[high];
	int i = low - 1;
	
	for(int j = low; j < high; j++) {
		// Sort by score (descending), then by ID (ascending)
		bool shouldSwap = false;
		if(arr[j].score > pivot.score) {
			shouldSwap = true;
		} else if(arr[j].score == pivot.score && arr[j].studentID < pivot.studentID) {
			shouldSwap = true;
		}
		
		if(shouldSwap) {
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return i + 1;
}

void quickSort(StudentScore arr[], int low, int high) {
	if(low < high) {
		int pi = partition(arr, low, high);
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

int suggest(int mID)
{
	bool globalSelected[MAX_STUDENTS+1] = {false};
	int result[MAX_STUDENTS+1] = {0};
	StudentScore candidates[MAX_STUDENTS];

	for (int u=1; u<=globalData.M; u++){
		int candidateCount = 0;
		
		// Collect all eligible students for this university
		for(int i=0; i<activeCount; i++){
			int m = activeStudents[i];
			if(!globalSelected[m] && isActive[m]) {
				candidates[candidateCount].studentID = m;
				candidates[candidateCount].score = universityScores[m][u];
				candidateCount++;
			}
		}
		
		if(candidateCount == 0) continue;
		
		// Sort candidates by score (desc) then by ID (asc)
		// Use simple selection sort for small arrays (more predictable than quicksort)
		for(int i = 0; i < candidateCount - 1; i++) {
			int maxIdx = i;
			for(int j = i + 1; j < candidateCount; j++) {
				bool isGreater = false;
				if(candidates[j].score > candidates[maxIdx].score) {
					isGreater = true;
				} else if(candidates[j].score == candidates[maxIdx].score && 
						  candidates[j].studentID < candidates[maxIdx].studentID) {
					isGreater = true;
				}
				
				if(isGreater) {
					maxIdx = j;
				}
			}
			if(maxIdx != i) {
				swap(&candidates[i], &candidates[maxIdx]);
			}
		}
		
		// Select top N students
		int toSelect = (candidateCount < globalData.N) ? candidateCount : globalData.N;
		for(int i=0; i<toSelect; i++){
			int studentID = candidates[i].studentID;
			globalSelected[studentID] = true;
			result[studentID] = u;
		}
	}
	
	if(result[mID] != 0) {
		return result[mID];
	}
	else{
		return -1;
	}
}




