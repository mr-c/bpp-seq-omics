// SPDX-FileCopyrightText: The Bio++ Development Group
//
// SPDX-License-Identifier: CECILL-2.1

#ifndef _SEQUENCEFEATURE_H_
#define _SEQUENCEFEATURE_H_

// From the STL:
#include <string>
#include <map>
#include <set>
#include <algorithm>

// From bpp-core:
#include <Bpp/Clonable.h>
#include <Bpp/Numeric/Range.h>

/**
 * @mainpage
 *
 * @par
 * The bpp-seq-omics library adds 'omics' components to the bpp-seq library.
 * These components notably include the bpp::SequenceFeature class, for storing generic sequence features and their coordinates.
 * Parsers are provided for loading such features from a bpp::Gff file for instance, as well as tools for extracting the corresponding sequences (bpp::SequenceFeatureTools).
 *
 * @par
 * Next-Generation Sequencing data are also supported, with the bpp::Fastq parser, which instanciates bpp::SequenceWithQuality objects.
 * Genome alignments can be efficiently analyzed via the bpp::MafParser class and a large range of bpp::MafIterator classes.
 * Genome alignment blocks are stored in bpp::MafBlock instances, which are containers of bpp::MafSequence objects with their corresponding genome coordinates.
 */

namespace bpp
{
/**
 * @brief a coordinate range on a sequence.
 * Stores coordinates as a Range<size_t> object,
 * but also keep the strand information.
 */
class SeqRange :
  public Range<size_t>
{
private:
  char strand_;

public:
  /**
   * @param a First position
   * @param b Second position
   * @param strand The strand information. Can take one of the four values: '+' for positive strand, '-' for negative, '.' if not stranded or '?' if strandedness is relevant but unknown.
   */
  SeqRange(size_t a, size_t b, char strand = '.') :
    Range<size_t>(a, b), strand_(strand)
  {
    if (strand != '+' && strand != '-' && strand != '?' && strand != '.')
      strand_ = '.';
  }

  /**
   * @param range A range object
   * @param strand The strand information. Can take one of the four values: '+' for positive strand, '-' for negative, '.' if not stranded or '?' if strandedness is relevant but unknown.
   */
  SeqRange(const Range<size_t> range, char strand = '.') :
    Range<size_t>(range), strand_(strand)
  {
    if (strand != '+' && strand != '-' && strand != '?' && strand != '.')
      strand_ = '.';
  }

  SeqRange* clone() const { return new SeqRange(*this); }

public:
  virtual char getStrand() const { return strand_; }

  virtual bool isNegativeStrand() const { return strand_ == '-'; }
  virtual bool isStranded() const { return strand_ == '+' || strand_ == '-'; }
  virtual void invert()
  {
    if (isStranded())
    {
      if (isNegativeStrand())
      {
        strand_ = '+';
      }
      else
      {
        strand_ = '-';
      }
    }
  }
};

/**
 * @brief The base interface for sequence features.
 *
 * This interface provides the most generic information common to all features, that is:
 * - sequence reference
 * - procedure used to generate the information
 * - start and end positions. Coordinates are 0-based, of type [a,b[, so that if a==b,
 * the annotation is considered empty. A one-character annotation is then noted as start=12(included),end=13(excluded) for instance.
 *
 * Subclasses inheriting this interface provide specialisations for various types of annotations.
 * Depending on the file format of annotations, not all types of annotations may be supported.
 *
 * @author Julien Dutheil
 */
class SequenceFeature :
  public virtual Clonable
{
public:
  static const std::string NO_ATTRIBUTE_SET;

public:
  virtual SequenceFeature* clone() const = 0;

public:
  /**
   * @return The id of this feature.
   */
  virtual const std::string& getId() const = 0;
  /**
   * @param id A std::string representing the id.
   */
  virtual void setId(const std::string& id) = 0;

  /**
   * @return The id of the sequence on which this feature is based.
   */
  virtual const std::string& getSequenceId() const = 0;
  /**
   * @param id A std::string representing the id of the reference.
   */
  virtual void setSequenceId(const std::string& id) = 0;

  /**
   * @return A text intended to describe the algorithm or procedure used to generate the feature.
   */
  virtual const std::string& getSource() const = 0;
  /**
   * @param source A std::string representing the source of the feature.
   */
  virtual void setSource(const std::string& source) = 0;

  /**
   * @return A text describing the type of feature. Depending on the format, it can be restricted (for example, mRNA), or any text can be supplied (for example TFXX binding site).
   */
  virtual const std::string& getType() const = 0;
  /**
   * @param type A std::string representing the type of this feature.
   */
  virtual void setType(const std::string& type) = 0;

  /**
   * @return The starting position of the feature, 0-based, included.
   */
  virtual const size_t getStart() const = 0;

  /**
   * @return The ending position of the feature, 0-based, excluded.
   */
  virtual const size_t getEnd() const = 0;

  /**
   * @return The size of the feature.
   */
  virtual const size_t size() const
  {
    return getEnd() - getStart();
  }

  /**
   * @return True if the feature is stranded.
   */
  virtual bool isStranded() const = 0;

  /**
   * @return True if the sequence is coded on the negative strand. False if it is on the positive one or unknown.
   */
  virtual bool isNegativeStrand() const = 0;

  /**
   * Change the orientation of the feature.
   */
  virtual void invert() = 0;

  /**
   * @return Coordinates as a Range object.
   */
  virtual SeqRange getRange() const = 0;

  /**
   * @return Check if the feature is empty (start == end)
   */
  virtual bool isEmpty() const { return size() == 0; }

  /**
   * @return Check if the feature is a point annotation (start + 1 == end)
   */
  virtual bool isPoint() const { return size() == 1; }

  /**
   * @return True if the features overlap.
   */
  virtual bool overlap(const SequenceFeature& feat) const = 0;

  /**
   * @return True if the feature overlap with the given range (non-null intersection).
   */
  virtual bool overlap(const SeqRange& range) const = 0;

  /**
   * @return True if the feature fully contains the given range.
   */
  virtual bool includes(const SeqRange& range) const = 0;

  /**
   * @return True if the feature is fully contained in the given range.
   */
  virtual bool isIncludedIn(const SeqRange& range) const = 0;

  /**
   * @return The score associated to the feature (eg, an E-value or a P-value).
   */
  virtual const double& getScore() const = 0;
  /**
   * @param score A double representing the score of this feature.
   */
  virtual void setScore(double score) = 0;

  /**
   * @param  attribute The name of the attribute to retrieve.
   * @return The attribute with specified name (read only).
   */
  virtual const std::string& getAttribute(const std::string& attribute) const = 0;

  /**
   * @param  attribute The name of the attribute to retrieve.
   * @return The attribute with specified name.
   */
  virtual std::string& getAttribute(const std::string& attribute) = 0;

  /**
   * @return The list of all attributes available.
   */
  virtual std::set< std::string > getAttributeList() const = 0;

  /**
   * @brief Set the value of an attribute.
   *
   * @param attribute The name of the attribute to set.
   * @param value     The value of the attribute to set.
   */
  virtual void setAttribute(const std::string& attribute, const std::string& value) = 0;

  /**
   * @param attribute The name of the attribute to be removed.
   */
  virtual void removeAttribute(const std::string& attribute) = 0;
};

/**
 * @brief A very simple implementation of the SequenceFeature class.
 *
 * It uses a hash map for storing attributes.
 */
class BasicSequenceFeature :
  public SequenceFeature
{
protected:
  std::string id_;
  std::string sequenceId_;
  std::string source_;
  std::string type_;
  SeqRange range_;
  double score_;
  mutable std::map<std::string, std::string> attributes_;
  // SequenceFeatureSet subFeatures_;

public:
  BasicSequenceFeature() : id_(""), sequenceId_(""), source_(""), type_(""), range_(0, 0, '.'), score_(-1), attributes_() {}

  BasicSequenceFeature(
      const std::string& id,
      const std::string& seqId,
      const std::string& source,
      const std::string& type,
      size_t start,
      size_t end,
      char strand,
      double score = -1) :
    id_(id), sequenceId_(seqId), source_(source),
    type_(type), range_(start, end, strand), score_(score),
    attributes_()
    // attributes_(), subFeatures_()
  {}

  virtual BasicSequenceFeature* clone() const { return new BasicSequenceFeature(*this); }

public:
  const std::string& getId() const { return id_; }
  void setId(const std::string& id) { id_ = id; }
  const std::string& getSequenceId() const { return sequenceId_; }
  void setSequenceId(const std::string& sid) { sequenceId_ = sid; }
  const std::string& getSource() const { return source_; }
  void setSource(const std::string& source) { source_ = source; }
  const std::string& getType() const { return type_; }
  void setType(const std::string& type) { type_ = type; }
  const size_t getStart() const { return range_.begin(); }
  const size_t getEnd() const { return range_.end(); }
  bool isStranded() const { return range_.isStranded(); }
  bool isNegativeStrand() const { return range_.isNegativeStrand(); }
  void invert()
  {
    range_.invert();
  }
  const double& getScore() const { return score_; }
  void setScore(double score) { score_ = score; }

  const std::string& getAttribute(const std::string& attribute) const
  {
    std::map<std::string, std::string>::iterator it = attributes_.find(attribute);
    if (it != attributes_.end())
      return it->second;
    else
      return NO_ATTRIBUTE_SET;
  }

  std::string& getAttribute(const std::string& attribute)
  {
    return attributes_[attribute];
  }

  void setAttribute(const std::string& attribute, const std::string& value)
  {
    attributes_[attribute] = value;
  }

  std::set< std::string > getAttributeList() const
  {
    std::set< std::string > d;
    for (std::map<std::string, std::string>::iterator it = attributes_.begin(); it != attributes_.end(); it++)
    {
      d.insert(it->first);
    }
    return d;
  }

  void removeAttribute(const std::string& attribute)
  {
    std::map<std::string, std::string>::iterator it = attributes_.find(attribute);
    if (it != attributes_.end())
    {
      attributes_.erase(it);
    }
  }

  SeqRange getRange() const
  {
    return SeqRange(range_);
  }

  bool overlap(const SequenceFeature& feat) const
  {
    if (feat.getSequenceId() == sequenceId_)
    {
      return range_.overlap(feat.getRange());
    }
    return false;
  }

  bool overlap(const SeqRange& range) const
  {
    return range_.overlap(range);
  }

  virtual bool includes(const SeqRange& range) const
  {
    return range_.contains(range);
  }

  virtual bool isIncludedIn(const SeqRange& range) const
  {
    return range.contains(range_);
  }

  // const SequenceFeatureSet& getSubFeatures() const { return subFeatures; }
  // SequenceFeatureSet& getSubFeatures() { return subFeatures; }
};

/**
 * @brief A simple ensemble of sequence features.
 *
 * This class is at a draft stage, and further improvements are expected, notably
 * to allow proper indexation, nested features, etc.
 *
 * For now, it is mostly a vector of feature object, stored as pointers.
 * A few functions are provided for convenience.
 *
 * @author Julien Dutheil
 */
class SequenceFeatureSet
{
private:
  std::vector<SequenceFeature*> features_;

public:
  SequenceFeatureSet() : features_() {}

  virtual ~SequenceFeatureSet() { clear(); }

  SequenceFeatureSet(const SequenceFeatureSet& sfs) :
    features_()
  {
    for (std::vector<SequenceFeature*>::const_iterator it = sfs.features_.begin();
        it != sfs.features_.end();
        ++it)
    {
      features_.push_back((**it).clone());
    }
  }
  SequenceFeatureSet& operator=(const SequenceFeatureSet& sfs)
  {
    clear();
    for (std::vector<SequenceFeature*>::const_iterator it = sfs.features_.begin();
        it != sfs.features_.end();
        ++it)
    {
      features_.push_back((**it).clone());
    }
    return *this;
  }

public:
  /**
   * @brief Delete all features in this set.
   */
  void clear()
  {
    for (std::vector<SequenceFeature*>::iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      delete *it;
    }
    features_.clear();
  }

  /**
   * @param i The index of the feature.
   * @return A reference toward the feature.
   */
  const SequenceFeature& getFeature(size_t i) const
  {
    return *features_[i];
  }

  /**
   * @param i The index of the feature.
   * @return A reference toward the feature.
   * @see getFeature(size_t i)
   */
  const SequenceFeature& operator[](size_t i) const
  {
    return *features_[i];
  }

  /**
   * @return The number of features in this set.
   */
  size_t getNumberOfFeatures() const { return features_.size(); }

  /**
   * @return True if the set contains no feature.
   */
  bool isEmpty() const { return features_.size() == 0; }

  /**
   * @brief Add a feature to the container. The feature will be copied and the copy owned by the container.
   *
   * @param feature The feature to add to the container.
   */
  void addFeature(const SequenceFeature& feature)
  {
    features_.push_back(feature.clone());
  }

  /**
   * @return A set containing all sequences ids in this set.
   */
  std::set<std::string> getSequences() const
  {
    std::set<std::string> seqIds;
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      seqIds.insert((**it).getSequenceId());
    }
    return seqIds;
  }

  /**
   * @return A set containing all feature type in this set.
   */
  std::set<std::string> getTypes() const
  {
    std::set<std::string> types;
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      types.insert((**it).getType());
    }
    return types;
  }

  /**
   * @brief Get all coordinates of features.
   * All ranges are added to a RangeCollection container, as SeqRange objects.
   * @param coords [out] a container where to add the coordinates of each feature.
   */
  void fillRangeCollection(RangeCollection<size_t>& coords) const
  {
    for (const auto it : features_)
    {
      coords.addRange(it->getRange());
    }
  }

  /**
   * @brief Get all coordinates of features for a given source.
   * All ranges are added to a RangeCollection container, as SeqRange objects.
   * @param seqId The name of the sequence id to consider.
   * @param coords [out] a container where to add the coordinates of each feature.
   */
  void fillRangeCollectionForSequence(const std::string& seqId, RangeCollection<size_t>& coords) const
  {
    for (const auto it : features_)
    {
      if (it->getSequenceId() == seqId)
      {
        coords.addRange(it->getRange());
      }
    }
  }

  /**
   * @param type The feature type.
   * @return A new set with all features of a given type.
   */
  SequenceFeatureSet* getSubsetForType(const std::string& type) const
  {
    SequenceFeatureSet* subset = new SequenceFeatureSet();
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      if ((**it).getType() == type)
      {
        subset->addFeature(**it);
      }
    }
    return subset;
  }

  /**
   * @param types The feature types.
   * @return A new set with all features of given types.
   */
  SequenceFeatureSet* getSubsetForTypes(const std::vector<std::string>& types) const
  {
    SequenceFeatureSet* subset = new SequenceFeatureSet();
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      if (std::find(types.begin(), types.end(), (**it).getType()) != types.end())
      {
        subset->addFeature(**it);
      }
    }
    return subset;
  }

  /**
   * @param id The sequence id to look for.
   * @return A new set with all features for a given sequence id.
   */
  SequenceFeatureSet* getSubsetForSequence(const std::string& id) const
  {
    SequenceFeatureSet* subset = new SequenceFeatureSet();
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      if ((**it).getSequenceId() == id)
      {
        subset->addFeature(**it);
      }
    }
    return subset;
  }

  /**
   * @param ids The sequence ids to look for.
   * @return A new set with all features of given sequence ids.
   */
  SequenceFeatureSet* getSubsetForSequences(const std::vector<std::string>& ids) const
  {
    SequenceFeatureSet* subset = new SequenceFeatureSet();
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      if (std::find(ids.begin(), ids.end(), (**it).getSequenceId()) != ids.end())
      {
        subset->addFeature(**it);
      }
    }
    return subset;
  }

  /**
   * @param range    The range of features to look for.
   * @param complete If true, only return features fully included in the given range.
   *                 Otherwise returns features overlapping with the range.
   * @return A new set with all features included in the given range.
   */
  SequenceFeatureSet* getSubsetForRange(const SeqRange& range, bool complete) const
  {
    SequenceFeatureSet* subset = new SequenceFeatureSet();
    for (std::vector<SequenceFeature*>::const_iterator it = features_.begin();
        it != features_.end();
        ++it)
    {
      if (complete)
      {
        if ((**it).isIncludedIn(range))
          subset->addFeature(**it);
      }
      else
      {
        if ((**it).overlap(range))
          subset->addFeature(**it);
      }
    }
    return subset;
  }
};
} // end of namespace bpp

#endif // _SEQUENCEFEATURE_H_
