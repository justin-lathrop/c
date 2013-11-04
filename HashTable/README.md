<h3>HashTable</h3>
<hr />


A pure C way to make a hash table to hold generic data and use a generic key.


The example use of the hashtable is simple but is to show the resizing capability 
within the hashtable.  Every hashtable is O(1) on lookups, but this is with optimal 
hardware and minimal to no collisions.  So we have a hashtable that can start at a 
small size and when added elements to it, it can expand to hold them with less 
collisions.


Configural parameters in the hashtable:
<ul>
	<li><b>Starting size:</b> Size the table will start at.</li>
	<li><b>Values to hold:</b> Cast every value inserted into the hashtable as void * and you 
	can use any value under the sun.</li>
	<li><b>Key to search with:</b> Key to do hashes on and search with.  Can be anything becuase 
	the user needs to define a hash function for the hashtable returning the hash to use.</li>
	<li><b>Max collisions:</b> Max number of collisions before table will resize to keep it 
	at an optimal size for quickest lookups.</li>
	<li><b>Growth factor:</b> The amount of entries to add onto the size if the max number 
	of collisions happen.  If the size of the table is large then the amount of time a 
	resize can take can be a little while.. but worth it in most cases.</li>
</ul>
