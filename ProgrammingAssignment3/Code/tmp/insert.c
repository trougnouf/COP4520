slNode * slInsert(slNode * slHead, uint32_t key)
{
	// Verify that new key is valid
	if(newKey >= MAXKEY || newKey <= MINKEY)	return NULL;
	
	// Determine # of levels
	uint8_t numLv = flipcoins()-1;
	
	// Initialize variables
	slNode * curNode = slHead;
	slNode * nextNode;
	int8_t lv;
	
	// Initialize new node
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(atomic_uintptr_t)*(numLv+1));
	newNode->key = newKey;
	
	// Go through each level
	for(slLEVELS-1;lv>=0;lv--)
	{
		slInserting:	// Come back without decrementing lv
		
		// Move right
		while(getPtr(curNode->next[lv])->key < key)
		{
			curNode = getPtr(curNode->next[lv]);
		}
		nextNode = getPtr(curNode->next[lv]);
		
		// Duplicate found
		if(nextNode->key == key)
		{
			// Insertion not started? Abort
			if(lv >= numLv)
			{
				free(newNode->next);
				free(newNode);
				return NULL;
			}
			// Insertion started? Merge
			for(;lv >= 0; lv--)
			{
				// Mark node for deletion
				while(atomic_fetch_or(&(nextNode->next[lv]), 1) & 1)
					printf("Merge: bit stealing failed.\n");
				newNode->next[lv] = (nextNode->next[lv]) & (UINTPTR_MAX ^ 1);
				if(!atomic_compare_exchange_strong(&(curNode->next[lv]), &nextNode, (uintptr_t)newNode))
				{
					printf("Merge: CAS failed.\n");
					// CAS failed. Reset flag and start over.
					nextNode->next[lv] &= (UINTPTR_MAX ^ 1);
					goto slInserting;
				}
			}
			free(nextNode->next);
			free(nextNode);
			break;
		}
		
		// Insert
		newNode->next[lv] = nextNode;
		if(!atomic_compare_exchange_strong(&(curNode->next[lv]), &nextNode, (uintptr_t)newNode));
			// Insert failed: start over.
			goto slInsertion;
	}
	if(numLv == slLEVELS-1)	return newNode;
	else	return NULL;
}

