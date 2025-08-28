/**
 * @file ox_list.h
 * @brief Doubly-linked list implementation
 */

#pragma once

/**
 * @brief Get the struct for this entry.
 * @param address The address of the list entry structure.
 * @param type The type of the struct this is embedded in.
 * @param field The name of the list_entry within the struct.
 * @return A pointer to the containing structure.
 */
#define ox_list_offset(address, type, field)                                   \
  ((type*)((char*)(address) - (char*)(&((type*)0)->field)))

/**
 * @brief Iterate over a list.
 * @param position The &struct ox_list_entry to use as a loop cursor.
 * @param head The head for your list.
 */
#define ox_list_for_each(position, head)                                       \
  for (position = (head)->next; position != head; position = position->next)

/**
 * @brief Iterate over a list safe against removal of list entry.
 * @param position The &struct ox_list_entry to use as a loop cursor.
 * @param n Another &struct ox_list_entry to use as temporary storage.
 * @param head The head for your list.
 */
#define ox_list_for_each_safe(position, n, head)                               \
  for (position = (head)->next, n = position->next; position != (head);        \
       position = n, n = position->next)

/**
 * @brief Doubly-linked list entry structure
 */
typedef struct ox_list_entry {
  struct ox_list_entry* prev; /**< Previous entry */
  struct ox_list_entry* next; /**< Next entry */
} ox_list_entry_t;

/** @brief List head type */
typedef ox_list_entry_t ox_list_head_t;

/**
 * @brief Initialize a list head
 * @param head List head to initialize
 */
void ox_list_init(ox_list_head_t* head);

/**
 * @brief Add entry to the beginning of the list
 * @param head List head
 * @param entry Entry to add
 */
void ox_list_add_head(ox_list_head_t* head, ox_list_entry_t* entry);

/**
 * @brief Add entry to the end of the list
 * @param head List head
 * @param entry Entry to add
 */
void ox_list_add_tail(ox_list_head_t* head, ox_list_entry_t* entry);

/**
 * @brief Remove entry from its list
 * @param entry Entry to remove
 */
void ox_list_remove(const ox_list_entry_t* entry);

/**
 * @brief Check if list is empty
 * @param head List head to check
 * @return Non-zero if empty, zero if not empty
 */
long ox_list_empty(const ox_list_entry_t* head);