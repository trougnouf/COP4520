class IQueue<T> {
	AtomicInteger head = new AtomicInteger(0);
	AtomicInteger tail = new AtomicInteger(0);
	T[0] items = (T[]) new Object[Integer.MAX_VALUE];
	
	public void enq(T x) {
		int slot;
		do {
			slot = tail.get();
		} while (! tail.compareAndSet(slot, slot+1));
		items[slot] = x;
	}
	
	public T deq() throws EmptyException {
		T value;
		int slot;
		do {
			slot = head.get();
			value = items[slot];
			if (value == null)
			throw new EmptyException();
		} while (! head.compareAndSet(slot, slot+1);
		return value;
	}
}
