#ifndef DATAFORMAT_DATABLOCKCONTAINER
#define DATAFORMAT_DATABLOCKCONTAINER

#include <Common/MemPool.h>
#include <Common/DataBlock.h>

#include <stdlib.h>
#include <memory>
#include <functional>

// A container class for data blocks.
// In particular, allows to take care of the block release after use.

class DataBlockContainer {

  public:
  DataBlockContainer(DataBlock *v_data=NULL);
  virtual ~DataBlockContainer();
  DataBlock *getData();

  using ReleaseCallback = std::function<void(void)>;
  // NB: may use std::bind to add extra arguments
  
  // this constructor allows to specify a callback which is invoked when container is destroyed
  DataBlockContainer(ReleaseCallback callback, DataBlock *v_data=NULL);

  protected:
  DataBlock *data;
  ReleaseCallback releaseCallback;
};




class DataBlockContainerFromMemPool : public DataBlockContainer {

  public:
  DataBlockContainerFromMemPool(std::shared_ptr<MemPool> pool, DataBlock *v_data=NULL);
  ~DataBlockContainerFromMemPool();

  private:
  std::shared_ptr<MemPool> mp;
};


/**
 * DataBlockContainer that takes ownership of the payload and deletes it when needed.
 */
class SelfReleasingBlockContainer : public DataBlockContainer
{

  public:
    SelfReleasingBlockContainer()
    {
      data = new DataBlock();
      data->data = nullptr;
    }

    ~SelfReleasingBlockContainer()
    {
      if (data->data != nullptr) {
        delete[] data->data;
      }
      delete data;
    }
};

#endif
