
#include "cache_insert_policy.h"

class CacheInsertEndpoint : public CacheInsertPolicy {
public:

    CacheInsertEndpoint() {
    }

    void insert(OverlayID *key, HostAddress *value) {
        DLLNode *node = new DLLNode(key, value);
        dll->append2Head(node);
        hm->add(*key, node);
    }
};