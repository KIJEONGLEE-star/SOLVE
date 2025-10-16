#include <stdio.h>

#define CMD_INIT 100
#define CMD_ADD 200
#define CMD_ERASE 300
#define CMD_SUGGEST 400

extern void init(int N, int M, int mWeights[][5]);
extern void add(int mID, int mScores[5]);
extern void erase(int mID);
extern int suggest(int mID);

#define MAX_M 30

static int mWeights[MAX_M][5];

static bool run()
{
	int Q;
	int N, M;
	int mID, mScores[5];
	int ret = -1, ans;
	scanf("%d", &Q);
	bool okay = false;

	for(int q=0; q<Q; ++q)
	{
		int cmd;
		scanf("%d", &cmd);
		switch(cmd)
		{
			case CMD_INIT:
				scanf("%d %d", &N, &M);
				for(int i=0; i<M; ++i)
					for(int j=0; j<5; ++j)
						scanf("%d", &mWeights[i][j]);
				init(N, M, mWeights);
				okay = true;
				break;
			case CMD_ADD:
				scanf("%d", &mID);
				for(int i=0; i<5; ++i)
					scanf("%d", &mScores[i]);
				add(mID, mScores);
				break;
			case CMD_ERASE:
				scanf("%d", &mID);
				erase(mID);
				break;
			case CMD_SUGGEST:
				scanf("%d", &mID);
				ret = suggest(mID);
				scanf("%d", &ans);
				if(ret!=ans)
					okay = false;
				break;
			default:
				okay = false;
				break;
		}
	}return okay;
}

int main()
{
	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);

	int TC, MARK;

	scanf("%d %d", &TC, &MARK);
	for(int tc=1; tc<=TC; ++tc)
	{
		int score = run() ? MARK : 0;
		printf("#%d %d\n", tc, score);
	}
	return 0;
}

