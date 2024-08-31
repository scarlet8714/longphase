#ifndef PHASINGGRAPH_H
#define PHASINGGRAPH_H

#include "Util.h"
#include "ParsingBam.h"
#include "PhasingProcess.h"


typedef std::pair<int, int> PosAllele;
typedef std::map<std::string, int> ReadBaseMap;

class SubEdge{
    
    private:
        int readCount;
        // Edge information. The vector store next pos
        // < next position, read name >
        std::map<int, std::vector<std::string> > *refRead;
        std::map<int, std::vector<std::string> > *altRead;
        // sum of edge pair quality, pos1 quality + pos2 quality
        // < next position, quality sum >
        std::map<int, int> *refQuality;
        std::map<int, int> *altQuality;
        // < next position, read count >
        std::map<int, float> *refReadCount;
        std::map<int, float> *altReadCount;

    public:
        
        SubEdge();
        ~SubEdge();
        
        void destroy();
        
        void addSubEdge(int currentQuality, Variant connectNode, std::string readName, int baseQuality, double edgeWeight,bool fakeRead);
        std::pair<float,float> BestPair(int targetPos);
        float getRefReadCount(int targetPos);
        float getAltReadCount(int targetPos);        
        
        std::vector<std::string> showEdge(std::string message);
        std::vector<std::pair<int,int>> getConnectPos();
 
        int getQuality(PosAllele targetPos);
        int getAvgQuality(PosAllele targetPos);

};

//use to store the voting info from the previous variants
struct VoteResult{
    int Pos;		//who votes
    float para;		//rr+aa
    float cross;	//ra+ar
    float weight;	//how much weight
    int hap;		//which haplotype 
    double ESR;		//similarity of para and cross

    VoteResult( int currPos, float weight ) ;
};

struct VariantEdge{
    int currPos;
    SubEdge* alt;
    SubEdge* ref;
    int refcnt ; // count the ref base amount
    int altcnt ; // count the alt base amount
    double vaf ; // count the vaf of the left snp
    int coverage ; // count the coverge on the snp
    
    VariantEdge(int currPos);
    // node pair 
    std::pair<PosAllele,PosAllele> findBestEdgePair(int targetPos, bool isONT, double diffRatioThreshold, bool debug, std::map<int,int> &variantType, VoteResult &vote);
    // number of read of two node. AA and AB combination
    std::pair<float,float> findNumberOfRead(int targetPos);
    bool get_fakeSnp();
};


struct BlockRead{
    std::map<std::string,int> readVec;
    
    void recordRead(std::string readName);
};

struct EdgeResult{
    int rr;
    int ra;
    int ar;
    int aa;
};

class VairiantGraph{
    
    private:
        PhasingParameters *params;
        std::string *ref;
        std::vector<std::string> dotResult;
        std::vector<ReadVariant> *readVariant;
        
        // By default, a Map in C++ is sorted in increasing order based on its key.
        // position, edge
        std::map<int,VariantEdge*> *edgeList;
        // Each position will record the included reads and their corresponding base qualities.
        // position, < read name, quality>
        std::map<int,ReadBaseMap*> *totalVariantInfo;
        // position, type < 0=SNP 1=SV 2=MOD 3=INDEL >
        std::map<int,int> *variantType;
 
        std::pair<float,float> Onelongcase( std::vector<VoteResult> vote ) ;

        // phasing result     
        // PosAllele , block_start    
        std::map<PosAllele,int> *bkResult;
        // record each position haplotype
        std::map<PosAllele,int> *subNodeHP;
        // store phased read and read's haplotype
        std::map<std::string,int> *readHpMap;

        // produce PS tag and determine phased GT tag
        void storeResultPath();
        
        void readCorrection();

        void edgeConnectResult();
        

    public:
    
        VairiantGraph(std::string &ref, PhasingParameters &params);
        ~VairiantGraph();
    
        void addEdge(std::vector<ReadVariant> &in_readVariant);
        
        void phasingProcess();
        void writingDotFile(std::string dotPrefix);
        std::map<std::string,int>* getReadHP();
        void exportResult(std::string chrName, PhasingResult &result);
        int totalNode();

        void destroy();
        
};




#endif
