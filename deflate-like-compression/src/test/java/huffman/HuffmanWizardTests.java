package test.java.huffman;

import main.java.huffman.HuffmanWizard;
import org.junit.jupiter.api.Test;

import java.io.*;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

/**
 * Created by theartful on 4/14/18.
 */

class HuffmanWizardTests {

    @Test
    void compressAndDecompressTest1() {
        compressAndDecompressFile("test.bmp", "test.bin", "test-output.bmp",
                16, 65536 * 16);
    }

    @Test
    void compressAndDecompressTest2() {
        compressAndDecompressFile("test2.bmp", "test2.bin", "test2-output.bmp",
                8, 65536);
    }

    private void compressAndDecompressFile(String fileName, String compressedFileName, String decompressedFileName,
                                           int wordLength, int blockSize) {
        try {
            FileInputStream fis = new FileInputStream(fileName);
            FileOutputStream fos = new FileOutputStream(compressedFileName);
            HuffmanWizard.encode(fis, fos, wordLength, blockSize);

            fis.close();
            fos.close();

            fis = new FileInputStream(compressedFileName);
            fos = new FileOutputStream(decompressedFileName);
            HuffmanWizard.decodeFile(fis, fos, wordLength);

            fis.close();
            fos.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            fail("Exception occured");
            return;
        } catch (IOException e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        Process p;
        try {
            p = Runtime.getRuntime().exec("diff " + fileName + " " + decompressedFileName);
            BufferedReader br = new BufferedReader(
                    new InputStreamReader(p.getInputStream()));
            assertEquals(null, br.readLine());
            p.waitFor();
            p.destroy();
        } catch (Exception e) {
            fail("Exception");
        }
    }

}
