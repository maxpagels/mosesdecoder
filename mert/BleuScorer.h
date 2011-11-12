#ifndef __BLEUSCORER_H__
#define __BLEUSCORER_H__

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits.h>
#include "Types.h"
#include "ScoreData.h"
#include "Scorer.h"
#include "ScopedVector.h"

using namespace std;

enum BleuReferenceLengthStrategy { BLEU_AVERAGE, BLEU_SHORTEST, BLEU_CLOSEST };


/**
 * Bleu scoring
 */
class BleuScorer: public StatisticsBasedScorer
{
public:
  explicit BleuScorer(const string& config = "");
  ~BleuScorer();

  virtual void setReferenceFiles(const vector<string>& referenceFiles);
  virtual void prepareStats(size_t sid, const string& text, ScoreStats& entry);

  size_t NumberOfScores() {
    // cerr << "BleuScorer: " << (2 * LENGTH + 1) << endl;
    return (2 * kLENGTH + 1);
  }


//protected:
  float calculateScore(const vector<int>& comps);

private:
  // no copying allowed
  BleuScorer(const BleuScorer&);
  BleuScorer& operator=(const BleuScorer&);

  //Used to construct the ngram map
  struct CompareNgrams {
    int operator() (const vector<int>& a, const vector<int>& b) {
      size_t i;
      size_t as = a.size();
      size_t bs = b.size();
      for (i = 0; i < as && i < bs; ++i) {
        if (a[i] < b[i]) {
          //cerr << "true" << endl;
          return true;
        }
        if (a[i] > b[i]) {
          //cerr << "false" << endl;
          return false;
        }
      }
      //entries are equal, shortest wins
      return as < bs;;
    }
  };

  typedef map<vector<int>,int,CompareNgrams> counts_t;
  typedef map<vector<int>,int,CompareNgrams>::iterator counts_it;

  typedef ScopedVector<counts_t> refcounts_t;

  /**
   * Count the ngrams of each type, up to the given length in the input line.
   */
  size_t countNgrams(const string& line, counts_t& counts, unsigned int n);

  void dump_counts(counts_t& counts) {
    for (counts_it i = counts.begin(); i != counts.end(); ++i) {
      cerr << "(";
      copy(i->first.begin(), i->first.end(), ostream_iterator<int>(cerr," "));
      cerr << ") " << i->second << ", ";
    }
    cerr << endl;
  }

  const int kLENGTH;
  BleuReferenceLengthStrategy _refLengthStrategy;

  // data extracted from reference files
  refcounts_t _refcounts;
  vector<vector<size_t> > _reflengths;
};

#endif  // __BLEUSCORER_H__
