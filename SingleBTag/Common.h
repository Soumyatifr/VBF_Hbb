
TString cuts[5] = {"bdtout<0.0",
		   "bdtout>=0.0&&bdtout<0.3",
		   "bdtout>=0.3&&bdtout<0.6",
		   "bdtout>=0.6&&bdtout<0.8",
		   "bdtout>=0.8"};
  
TString names[5] = {"CAT4",
		    "CAT5",
		    "CAT6",
		    "CAT7",
		    "CAT8"};

int NbinsBkg = 150;
int NbinsSig =  30;
int Nbins = 1500;
float xmin = 80;
float xmax = 230;
