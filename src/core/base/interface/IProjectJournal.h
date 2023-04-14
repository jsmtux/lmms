#ifndef D21A31E8_45A0_4B4E_859A_6DBA0E7BECE1
#define D21A31E8_45A0_4B4E_859A_6DBA0E7BECE1

#include "lmms_basics.h"

namespace lmms
{

class JournallingObject;

class IProjectJournal {
public:
    virtual ~IProjectJournal() = default;
    virtual jo_id_t allocID( JournallingObject * _obj ) = 0;
    virtual void freeID( const jo_id_t _id ) = 0;
    virtual void reallocID( const jo_id_t _id, JournallingObject * _obj ) = 0;
    virtual void addJournalCheckPoint( JournallingObject *jo ) = 0;
    virtual JournallingObject * journallingObject( const jo_id_t _id ) = 0;
};

}

#endif /* D21A31E8_45A0_4B4E_859A_6DBA0E7BECE1 */
