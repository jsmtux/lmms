#pragma once

#include "lmms_basics.h"
#include "JournallingObject.h"

namespace lmms
{

class IProjectJournal
{
public:
    virtual jo_id_t allocID( JournallingObject * _obj ) = 0;
    virtual void reallocID( const jo_id_t _id, JournallingObject * _obj ) = 0;
    virtual void freeID( const jo_id_t _id ) = 0;
    virtual void addJournalCheckPoint( JournallingObject *jo ) = 0;
    virtual JournallingObject * journallingObject( const jo_id_t _id ) = 0;
};

}