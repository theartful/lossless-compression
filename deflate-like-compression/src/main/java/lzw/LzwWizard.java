package main.java.lzw;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Iterator;

/**
 * Created by theartful on 4/14/18.
 */
public class LzwWizard {

    static final int SLIDING_WINDOW_SIZE = 17;
    static final int BUFFER_BITS_NUM = 6;

    static final int MINIMUM_MATCH_SIZE = 3;

    public static void encodeFile(InputStream is, OutputStream os) throws IOException {
        int slidingWindowSize = 1 << SLIDING_WINDOW_SIZE;
        int lookAheadBufferSize = (1 << BUFFER_BITS_NUM) + MINIMUM_MATCH_SIZE - 1;

        Buffer slidingWindow = new Buffer(slidingWindowSize);
        Buffer lookAheadBuffer = new Buffer(lookAheadBufferSize);

        // populate lookAheadBuffer
        byte[] lookAheadFirstPatch = new byte[lookAheadBufferSize];
        int actualRead = is.read(lookAheadFirstPatch);

        for (int i = 0; i < actualRead; i++) lookAheadBuffer.addByte(lookAheadFirstPatch[i]);

        // memory[0] = the byte to be written
        // memory[1] = bitIndex
        int[] memory = new int[2];

        int processed = 0;
        byte first;
        int b;

        boolean p = false;
        
        while (!lookAheadBuffer.isEmpty()) {

            int[] longestMatch = findLongestMatch(lookAheadBuffer, slidingWindow);
            int distance = longestMatch[0];
            int length = longestMatch[1];

            //int distance = 0;
            //int length = 0;

            if (length >= MINIMUM_MATCH_SIZE) {
                System.out.println("length: " + length + " " + "distance " + distance);
                writeBits(os, 1, 1, memory);
                writeBits(os, distance, SLIDING_WINDOW_SIZE, memory);
                writeBits(os, length - MINIMUM_MATCH_SIZE, BUFFER_BITS_NUM, memory);
                if (((processed / 1024) + 1) * 1024 - processed <= length) p = true;
                processed += length;
                for (int i = 0; i < length - 1; i++) {
                    b = is.read();
                    first = lookAheadBuffer.removeFirst();
                    slidingWindow.addByte(first);
                    if (b != -1) {
                        lookAheadBuffer.addByte((byte) b);
                    }
                }

            } else {
                // write byte as is
                processed++;
                writeBits(os, 0, 1, memory);
                writeBits(os, lookAheadBuffer.getFirstByte(), 8, memory);
            }

            first = lookAheadBuffer.removeFirst();
            slidingWindow.addByte(first);
            b = is.read();
            if (b != -1) {
                lookAheadBuffer.addByte((byte) b);
            }
            if (processed % 1024 == 0 || p) {
                System.out.println(processed);
                p = false;
            }
        }
        writeBits(os, 0, 8 - memory[1], memory);
    }

    private static void writeBits(OutputStream os, int data, int length, int[] memory) throws IOException {
        int b = memory[0];
        int bitIndex = memory[1];

        for (int i = 0; i < length; i++) {
            int bit = (data >> i) & 1;
            b |= (bit << bitIndex);
            bitIndex++;
            if (bitIndex > 7) {
                os.write(b);
                b = 0;
                bitIndex = 0;
            }
        }
        memory[0] = b;
        memory[1] = bitIndex;
    }

    // distance, length pair
    private static int[] findLongestMatch(Buffer lookAheadBuffer, Buffer slidingWindow) {
        Iterator<Byte> lookAheadBufferIterator = lookAheadBuffer.getIterator();
        if (!lookAheadBufferIterator.hasNext()) return new int[]{0, 0};

        byte toBeMatched = lookAheadBufferIterator.next();

        int maxDistance = 0;
        int maxLength = 0;

        int distance = -1;
        int length = 0;

        int currentDistance = 0;


        for (Iterator<Byte> slidingWindowIterator = slidingWindow.getIterator(); slidingWindowIterator.hasNext(); ) {
            byte slidingWindowByte = slidingWindowIterator.next();
            if (slidingWindowByte == toBeMatched) {
                length++;
                if (distance == -1)
                    distance = currentDistance;

                if (!lookAheadBufferIterator.hasNext()) {
                    break;
                } else
                    toBeMatched = lookAheadBufferIterator.next();
            } else {
                if (length > maxLength) {
                    maxLength = length;
                    maxDistance = distance;
                }
                distance = -1;
                length = 0;
                lookAheadBufferIterator = lookAheadBuffer.getIterator();
                toBeMatched = lookAheadBufferIterator.next();
            }
            currentDistance++;
        }
        if (length > maxLength) {
            maxLength = length;
            maxDistance = distance;
        }
        return new int[]{maxDistance, maxLength};
    }

    public static void decodeFile(InputStream is, OutputStream os) throws Exception {
        int slidingWindowSize = 1 << SLIDING_WINDOW_SIZE;
        Buffer slidingWindow = new Buffer(slidingWindowSize);

        int readByte;
        int bitIndex = 0;

        final int UNSPECIFIED = -1;
        final int NORMAL_BYTE = 0;
        final int DISTANCE = 1;
        final int LENGTH = 2;

        int readMode = UNSPECIFIED;
        int length = 0;
        int distance = 0;
        int b = 0;

        int anotherBitIndex = 0;

        while ((readByte = is.read()) != -1) {
            while (bitIndex < 8) {
                int bit = (readByte >> bitIndex) & 1;
                if (readMode == UNSPECIFIED) {
                    if (bit == 0) readMode = NORMAL_BYTE;
                    else readMode = DISTANCE;
                    length = 0;
                    distance = 0;
                    b = 0;
                    anotherBitIndex = 0;
                } else if (readMode == DISTANCE) {
                    distance |= bit << anotherBitIndex;
                    anotherBitIndex++;
                    if (anotherBitIndex >= SLIDING_WINDOW_SIZE) {
                        readMode = LENGTH;
                        anotherBitIndex = 0;
                    }
                } else if (readMode == LENGTH) {
                    length |= bit << anotherBitIndex;
                    anotherBitIndex++;
                    if (anotherBitIndex >= BUFFER_BITS_NUM) {
                        length += MINIMUM_MATCH_SIZE;
                        byte[] bytes = new byte[length];
                        System.out.println("length: " + length + " distance: " + distance);
                        for (int i = distance; i < distance + length; i++) {
                            bytes[i - distance] = slidingWindow.getByte(i);
                            os.write(bytes[i - distance]);
                        }

                        for (int i = 0; i < length; i++) {
                            slidingWindow.addByte(bytes[i]);
                        }

                        readMode = UNSPECIFIED;
                        anotherBitIndex = 0;
                        length = 0;
                        distance = 0;
                    }
                } else if (readMode == NORMAL_BYTE) {
                    b |= bit << anotherBitIndex;
                    anotherBitIndex++;
                    if (anotherBitIndex > 7) {
                        slidingWindow.addByte((byte) b);
                        os.write(b);
                        readMode = UNSPECIFIED;
                        anotherBitIndex = 0;
                        b = 0;
                    }
                }
                bitIndex++;
            }
            bitIndex = 0;
        }

    }

}
