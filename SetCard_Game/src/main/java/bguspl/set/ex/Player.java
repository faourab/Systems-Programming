package bguspl.set.ex;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import bguspl.set.Env;

/**
 * This class manages the players' threads and data
 *
 * @inv id >= 0
 * @inv score >= 0
 */
public class Player implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;

    /**
     * The id of the player (starting from 0).
     */
    public final int id;

    /**
     * The thread representing the current player.
     */
    protected Thread playerThread;

    /**
     * The thread of the AI (computer) player (an additional thread used to generate
     * key presses).
     */
    private Thread aiThread;

    /**
     * True iff the player is human (not a computer player).
     */
    private final boolean human;

    /**
     * True iff game should be terminated.
     */
    private volatile boolean terminate;

    /**
     * The current score of the player.
     */
    private int score;

    protected Dealer dealer;
    public volatile boolean canPlay;
    public volatile boolean penalty;
    public volatile long FreezingTime;

    public LinkedBlockingQueue<Integer> presKeys = new LinkedBlockingQueue();
    public List<Integer> tokens = new LinkedList<>();
    public LinkedBlockingQueue<Boolean> answer = new LinkedBlockingQueue(1);
     public volatile boolean point;


    /**
     * The class constructor.
     *
     * @param env    - the environment object.
     * @param dealer - the dealer object.
     * @param table  - the table object.
     * @param id     - the id of the player.
     * @param human  - true iff the player is a human player (i.e. input is provided
     *               manually, via the keyboard).
     */

    public Player(Env env, Dealer dealer, Table table, int id, boolean human) {
        this.env = env;
        this.table = table;
        this.id = id;
        this.human = human;
        terminate = false;
        this.dealer = dealer;
        canPlay = true;
        penalty = false;
        point =false;
    }

    /**
     * The main player thread of each player starts here (main loop for the player
     * thread).
     */
    @Override
    public void run() {
        playerThread = Thread.currentThread();
        env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
        if (!human)
            createArtificialIntelligence();
        while (!terminate) {
            // TODO implement main player loop
            try {
                Integer slot = presKeys.take();
                if (tokens.contains(slot)) {
                    table.semas[slot].acquire();
                    tokens.remove(slot);
                    table.removeToken(this.id, slot);
                    table.semas[slot].release();
                    canPlay = true;
                } else if (!tokens.contains(slot) && tokens.size() < 3 && dealer.start) {
                    table.semas[slot].acquire();
                    tokens.add(slot);
                    table.placeToken(this.id, slot);
                    table.semas[slot].release();
                    if (tokens.size() == 3) {
                        dealer.addlegal(this);
                        answer.take();
                        if (!canPlay)
                            if (penalty) {
                                penalty();
                            } else if (point) {
                                point();
                            }
                    }
                }
            } catch (InterruptedException e) {
            }
        }
        if (!human) {
            aiThread.interrupt();
            try {
                aiThread.join();
            } catch (InterruptedException ignored) {
            }
        }
        env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * Creates an additional thread for an AI (computer) player. The main loop of
     * this thread repeatedly generates
     * key presses. If the queue of key presses is full, the thread waits until it
     * is not full.
     */
    private void createArtificialIntelligence() {
        // note: this is a very, very smart AI (!)
        aiThread = new Thread(() -> {
            env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
            while (!terminate) {
                // TODO implement player key press simulator
                Random random = new Random()  ;
                int val = random.nextInt(env.config.tableSize) ;
                if (canPlay) {
               if (this.tokens.size()<3||(this.tokens.size()==3&&!penalty)) {
                    keyPressed(val);
            }
        }
                try {
                    Thread.sleep(2);
                } catch (InterruptedException e) {}                  
        }   
            env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
        }, "computer-" + id);
        aiThread.start();
    }

    /**
     * Called when the game should be terminated.
     */
    public void terminate() {
        this.terminate = true;
        playerThread.interrupt();
        try {
            playerThread.join();
        } catch (InterruptedException e) {
        }
    }

    /**
     * This method is called when a key is pressed.
     *
     * @param slot - the slot corresponding to the key pressed.
     */
    public void keyPressed(int slot) {
        if (canPlay) {
            try {
                presKeys.put(slot);
            } catch (Exception e) {
            }
        }
    }

    /**
     * Award a point to a player and perform other related actions.
     *
     * @post - the player's score is increased by 1.
     * @post - the player's score is updated in the ui.
     */
    public void point() {
        // TODO implement
            int ignored = table.countCards(); // this part is just for demonstration in the unit tests
            env.ui.setScore(id, ++score);
            FreezingTime = env.config.pointFreezeMillis;
            canPlay = true;
            penalty = false;
            for (long i = FreezingTime; i > 0; i = i - 500) {
                env.ui.setFreeze(this.id, i);
                try {
                    if (i >= 500) {
                        Thread.sleep(500);
                    } else if (i > 0)
                        Thread.sleep(i);

                } catch (InterruptedException e) {
                }
            }
            env.ui.setFreeze(this.id, 0);
        point =false;

    }

    /**
     * Penalize a player and perform other related actions.
     */
    public void penalty() {
        FreezingTime = env.config.penaltyFreezeMillis;
        canPlay = true;
        penalty = false;
        for (long i = FreezingTime; i > 0; i = i - 500) {
            env.ui.setFreeze(this.id, i);
            try {
                if (i >= 500) {
                    Thread.sleep(500);
                } else if (i > 0)
                    Thread.sleep(i);

            } catch (InterruptedException e) {
            }
        }
        env.ui.setFreeze(this.id, 0);
    }

    public int[] cardsPressed() {

         int[] cards = new int[tokens.size()];
         
        for (int i = 0; i < tokens.size(); i++) {
            cards[i] = tokens.get(i);
        }
         return cards;
    }

    public int score() {
        return score;
    }

    public void wakeUp() {
        try{
        answer.put(true);
        } catch(Exception e){}
    }
}
