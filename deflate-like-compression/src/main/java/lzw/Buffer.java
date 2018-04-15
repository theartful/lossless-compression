package main.java.lzw;

import java.util.Iterator;
import java.util.LinkedList;

/**
 * Created by theartful on 4/14/18.
 */
public class Buffer {

    LinkedList<Byte> buffer;
    int bufferSize;

    Buffer(int bufferSize) {
        this.bufferSize = bufferSize;
        buffer = new LinkedList<>();
    }

    void addByte(byte b) {
        buffer.add(b);
        if (buffer.size() > bufferSize) {
            buffer.removeFirst();
        }
    }

    Iterator<Byte> getIterator() {
        return buffer.iterator();
    }

    byte getFirstByte() {
        return buffer.getFirst();
    }

    byte getByte(int index) {
        return buffer.get(index);
    }

    boolean isEmpty(){
        return buffer.isEmpty();
    }

    public byte removeFirst() {
        return buffer.removeFirst();
    }

    public int size() {
        return buffer.size();
    }
}
