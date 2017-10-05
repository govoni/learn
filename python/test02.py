# build a fixed size matrix of objects

class FractionJetHistograms :
    def __init__ (self, name) :
        self.name = name
        
pippo = tuple( tuple(FractionJetHistograms ('MatchedCleanJetsCompontents_%d_%d' % (j,i) ) for i in range (10) ) for j in range(10)  )

print pippo[1][2].name

