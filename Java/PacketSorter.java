package sorting;

import java.util.Comparator;

/**
 * Class full of static sorting methods. Used to sort packets received from a
 * server containing image metadata.
 */

public class PacketSorter {
    /**
     * Sorts the given array of packets in ascending order according to the
     * comparator using mergesort. You may create as many private helper
     * functions as you wish to implement this method.
     * 
     * A note about ascending order:
     * 
     * When the method is finished, it should be true that:
     * comparator.compare(array[i - 1], array[i]) <= 0 for all i from 1 through
     * array.length.
     * 
     * @param array
     *            the packets to sort
     * @param comparator
     *            The comparator the will be used to compare two packets.
     */
    public static void mergeSort(Packet[] array, Comparator<Packet> comparator) {
        mSorter(array, comparator);
    }

    private static Packet[] mSorter(Packet[] array, Comparator<Packet> comparator) {
        if (array.length < 2) {
            return array;
        } else {
            Packet[] left = mSorter(copyRange(array, 0, array.length / 2), comparator);
            Packet[] right = mSorter(copyRange(array, array.length / 2, array.length), comparator);
            return merge(array, left, right, comparator);
        }
    }

    private static Packet[] merge(Packet[] array, Packet[] left, Packet[] right, Comparator<Packet> comparator) {
        int lP = 0;
        int rP = 0;
        for (int i = 0; i < array.length; i++) {
            if (rP >= right.length) {
                array[i] = left[lP];
                lP++;
            } else if (lP >= left.length) {
                array[i] = right[rP];
                rP++;
            } else {
                if (comparator.compare(left[lP], right[rP]) > 0) {
                    array[i] = right[rP];
                    rP++;
                } else {
                    array[i] = left[lP];
                    lP++;
                }
            }
        }
        return array;
    }

    private static Packet[] copyRange(Packet[] array, int from, int to) {
        Packet[] ret = new Packet[to - from];
        for (int i = from; i < to; i++) {
            ret[i - from] = array[i];
        }
        return ret;
    }

    /**
     * Sort the array of packets in ascending order using selection sort.
     * 
     * A note about ascending order:
     * 
     * When the method is finished, it should be true that:
     * comparator.compare(array[i - 1], array[i]) <= 0 for all i from 1 through
     * array.length.
     * 
     * @param array
     *            the array of packets that will be sorted.
     * @param comparator
     *            The comparator the will be used to compare two packets.
     */
    public static void selectionSort(Packet[] array,
            Comparator<Packet> comparator) {
        int min = 0;
        for (int i = 0; i < array.length; i++) {
            min = i;
            for (int k = i; k < array.length; k++) {
                if (comparator.compare(array[min], array[k]) > 0) {
                    min = k;
                }
            }
            Packet temp = array[i];
            array[i] = array[min];
            array[min] = temp;
        }
    }

    /**
     * Sort the array of packets in ascending order using insertion sort.
     * 
     * A note about ascending order:
     * 
     * When the method is finished, it should be true that:
     * comparator.compare(array[i - 1], array[i]) <= 0 for all i from 1 through
     * array.length.
     * 
     * @param array
     *            the array of packets that will be sorted.
     * @param comparator
     *            The comparator the will be used to compare two packets.
     */
    public static void insertionSort(Packet[] array,
            Comparator<Packet> comparator) {
        for (int outerIndex = 1; outerIndex < array.length; outerIndex++) {
            Packet currentPacket = array[outerIndex];
            int innerIndex = outerIndex - 1;
            while (innerIndex >= 0
                    && comparator.compare(currentPacket, array[innerIndex]) < 0) {
                array[innerIndex + 1] = array[innerIndex];
                innerIndex--;
            }
            array[innerIndex + 1] = currentPacket;
        }
    }
}
