#include "ox_list.h"

static void ox_list_insert(ox_list_entry_t* _new, ox_list_entry_t* prev,
                           ox_list_entry_t* next)
{
  next->prev = _new;
  _new->next = next;
  _new->prev = prev;
  prev->next = _new;
}

void ox_list_init(ox_list_head_t* head)
{
  head->prev = head;
  head->next = head;
}

void ox_list_add_head(ox_list_head_t* head, ox_list_entry_t* entry)
{
  ox_list_insert(entry, head, head->next);
}

void ox_list_add_tail(ox_list_head_t* head, ox_list_entry_t* entry)
{
  ox_list_insert(entry, head->prev, head);
}

static void ox_list_remove2(ox_list_entry_t* prev, ox_list_entry_t* next)
{
  next->prev = prev;
  prev->next = next;
}

void ox_list_remove(const ox_list_entry_t* entry)
{
  ox_list_remove2(entry->prev, entry->next);
}

long ox_list_empty(const ox_list_entry_t* head)
{
  return head->next == head;
}
