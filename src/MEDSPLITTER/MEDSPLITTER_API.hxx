#ifndef MEDSPLITTER_API_HXX_
#define MEDSPLITTER_API_HXX_


extern "C" int medsplitter(const char* inputfilename, 
			   const char* mesh,
			   const char* outputfilename,  
			   int is_distributed,
			   int nprocs,
			   int method,
			   int meshonly);

#endif
