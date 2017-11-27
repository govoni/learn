#!/usr/bin/python
''' split a txt file using a list of words as subsequent separators:
    text0 word1  text1 word2 text2 word3 text3
    gets retured as [text0, text1, text2, text3] if the separator list given is
    [word1, word2, word3]
'''


# ------------------------------------------------------------------------------


def splitWithList (filename, keywords):
  result = []
  f = open(filename, 'r')
  filecontent = f.read ()
  start = []
  end = []
  start.append (0)
  for word in keywords:
    position = filecontent.find (word)
    if position != -1 :
        end.append (position)
        start.append (position+len (word))
  end.append (len (filecontent))
  for ind in range (0, len (start)):
    print start[ind], end[ind]
    result.append (filecontent[start[ind]:end[ind]])
  return result


# ------------------------------------------------------------------------------


if __name__ == '__main__':
  
  filename = 'split_example.txt'
  keywords = ['NORMALIZATION','ALFA(i) DETERMINATION','INTEGRATION'] 

#  filename = 'split_example_zero.txt'
#  keywords = ['WORD1','WORD2','WORD3'] 
#  keywords = ['WORD1','WORD2','WORD3','WORD4'] 
#  keywords = ['WORD1','pippo','WORD2','WORD3','WORD4'] 
  result = splitWithList (filename, keywords)
 
  for chunk in result:
      print '\n\n ========================== \n\n'
      print chunk
    


