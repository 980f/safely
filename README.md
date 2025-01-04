The 'safely' library is C++ code for writing bug free code.
It was written when C++11 was just getting adopted, and used it sparingly at first.
It is presently being updated to C++17, but will contain some classes that have equivalent ones in the library to allow incremental incorporation of them.
--
One of the critical classes for that is the Indexer template which takes a pointer and a length and makes it convenient to not access data outside that buffer.
Indexer's function naming needs work, most functions were named for first use but have in some cases been used quite differently than that first use.

Safely is layered. 'cppext' is things that don't use heap or printf, 'system' has common things that may use heap, 'posix' brings in files and other standard Unix features. 'network' is still dependent upon posix but hopefully will get transport independent features broken out, such as http protocol parsing and formatting.
'usb' presently depends upon linux specifics, the microcontroller stuff is very chip specific, 'rasbpi' is very specific to the raspberrypi.
Heh, usb is actually its own git repo!

The next facet of 'safely' is implementing change/event driven logic. On the system side the persistent storage mechanism uses sigc to invoke callbacks on changes to nominally persistent data. It is so convenient that I've been in the habit of creating persistent data structures with no backing storage just to use the callback system, and to add persistent storage to items just to have post-mortem debug of a process.

The persistence system (which is broken out into its own folder) has as a core naming fragment 'Stor' (Store,Storable,Stored). That might seem a bit vague, or that it is naming the system for an under-the-hood aspect of it, but is very easy to type. Additionally other more common names for this class of feature are overused. The Stored system was initially built around a JSON persistence mechanism, but is not tied to that. All the JSON nature is outside of the Stor* classes. The persistent unit is a key-value pair, and one value type is a simple ordered collection of such key-value pairs. Note that null keys are allowed and in collection values, the index into the collection keeps the values unique. Stor* does not use hashtables for searching for a key as it was empirically found that linear searches were faster than computing hashes. The only case where that wasn't true was for the UI localization mechanism for which an external object was created to expedite searching (think RDBMS index). The latter has not yet been field-tested as most UI screens had few enough text members, and the list of text was built by running the UI and as such the most frequently used items are at the head of the list. Someone later sorted the developed list of translation items for the ease of translation, but the performance hit was not felt by the users.

The worst weakness in the Storable base class is the non-const keys. That is a legacy of a bad use case that will be eliminated soon. That bad use case was using UI supplied text as keys, all such instances should use co-indexing between two sets of unkeyed values. The coindexing works as such: each StoredArray has callback lists for insertion events and deletion events. A common callback is one that adds or deletes an element in one array due to a similar change in another. There are convenience methods for making such hookups. In RDBMS terms once you have enabled referential integrity it cascades deletes, and insertions create new items not allowing there to be a null target to a reference. When an empty cell isn't the best choice for a coordinated insertion one creates a specific 'on insert' callback that will have available to it data from the object being inserted in the primary array for use in creating its coordinated object in the secondary array.

Glib was once used is this library, but is being purged. The final step is to tame POSIX timers for delayed execution, which is most often used to sequence actions in the same callback list.

StorableIndexer: fast lookup for translation tables, presently linear searches are used.
These hook into the change detect for additions and deletions and upon such events update the index.
It's a friend class so that it can get pointers to the keys' raw text.
