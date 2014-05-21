// CamStudio Open Source - Commandline Header File
// License: GPL
// Coded by karol(dot)toth(at)gmail(dot)com

class screen{
public:
	int index;
	int left;
	int right;
	int top;
	int bottom;
	int width;
	int height;
	char outFile[50];
	char dispName[50];
	screen(){
		
	};

	bool SetDimensions(int left, int right, int top, int bottom){
		if(right > left && bottom > top){
			this->left = left;
			this->right = right;
			this->top = top;
			this->bottom = bottom;
			this->width = (right - left) - 1;
			this->height = (bottom - top) - 1;
			return true;
		}
		else{
			return false;
		}

	}
	//bool SetName(string name);
protected:
};
