package bguspl.set.ex;

import bguspl.set.Env;

import java.lang.Thread.State;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * This class manages the dealer's threads and data
 */
public class Dealer implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;
    private final Player[] players;

    /**
     * The list of card ids that are left in the dealer's deck.
     */
    private final List<Integer> deck;

    /**
     * True iff game should be terminated.
     */
    private volatile boolean terminate;

    /**
     * The time when the dealer needs to reshuffle the deck due to turn timeout.
     */
    private long reshuffleTime = Long.MAX_VALUE;

    private LinkedBlockingQueue<Player> submissions;
    private Player currentPlayer;
    protected volatile boolean start;

    public Dealer(Env env, Table table, Player[] players) {
        this.env = env;
        this.table = table;
        this.players = players;
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
        submissions = new LinkedBlockingQueue<>();
    }
    /**
     * The dealer thread starts here (main loop for the dealer thread).
     */
    @Override
    public void run() {
        for (Player p : players) {
            Thread playerThread = new Thread(p);
            playerThread.setName("Player Thread.id: " + p.id);
            playerThread.start();
        }
        env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
        while(!shouldFinish()) {
            updateTimerDisplay(true);
            placeCardsOnTable();
            updateTimerDisplay(true);
            timerLoop();
            removeAllCardsFromTable();
        }
        removeAllCardsFromTable();
        terminateall();
        announceWinners();
        env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * The inner loop of the dealer thread that runs as long as the countdown did
     * not time out.
     */
    private void timerLoop() {
        while (!terminate && System.currentTimeMillis() < reshuffleTime) {
            sleepUntilWokenOrTimeout();
            updateTimerDisplay(false);
            if (checksets()) {
                placeCardsOnTable();
            }
            if(table.cardsontable.size() == 0 && env.util.findSets(deck,1).size()==0||deck.size() == 0 && env.util.findSets(table.cardsontable,1).size()==0){break;}
        }
    }
    


    /**
     * Called when the game should be terminated.
     */
    public void terminate() {
        terminate = true;
    }

    /**
     * Check if the game should be terminated or the game end conditions are met.
     *
     * @return true iff the game should be finished.
     */
    private  boolean shouldFinish() {
        List<Integer> all = new ArrayList<>();
        for (int i = 0; i < table.cardsontable.size(); i++) {
            all.add(table.cardsontable.get(i)); 
        }
        for (int i = 0; i < deck.size(); i++) {
            all.add(deck.get(i)); 
        }
    return terminate ||env.util.findSets(all, 1).size() == 0;
    }
    /**
     * Checks cards should be removed from the table and removes them.
     */

    private void removeCardsFromTable() {
        // TODO implement
    }
    
    /**
     * Check if any cards can be removed from the deck and placed on the table.
     */
    private void placeCardsOnTable() {

        // TODO implement
        start =false;

            for (int i = 0; i < env.config.tableSize ; i++) {
                if (table.slotToCard[i] == null && !deck.isEmpty() && deck.get(0)!=null) {
                    Collections.shuffle(deck);
                    Collections.shuffle(deck);
                    Integer card = deck.remove(0);
                    table.placeCard(card, i);
                }
        }
        start = true;
    }

    /**
     * Sleep for a fixed amount of time or until the thread is awakened for some
     * purpose.
     */
    private void sleepUntilWokenOrTimeout() {
        // TODO implement
        if (reshuffleTime - System.currentTimeMillis() <= env.config.turnTimeoutWarningMillis) {
            currentPlayer = submissions.poll();
        } else {
            try {
                currentPlayer = submissions.poll(1000, TimeUnit.MILLISECONDS);
            } catch (InterruptedException e) {
            }
        }
    }

    /**
     * Reset and/or update the countdown and the countdown display.
     */
    private void updateTimerDisplay(boolean reset) {
        // TODO implement
        if (reset) {
            reshuffleTime = System.currentTimeMillis() + env.config.turnTimeoutMillis;
            env.ui.setCountdown(env.config.turnTimeoutMillis, false);
        } 
        else {
            boolean warn = false;
            long currentime = reshuffleTime - System.currentTimeMillis();
            if (currentime <= env.config.turnTimeoutWarningMillis) {
                warn = true;
            }
            if (currentime <= 0) {
                env.ui.setCountdown(0, warn);
            } else {
                env.ui.setCountdown(currentime, warn);
            }
        }
    }

    /**
     * Returns all the cards from the table to the deck.
     */
    private void removeAllCardsFromTable() {
        // TODO implement
        start =false;
    for (int i = 0; i < env.config.tableSize; i++) {    
        Integer card = table.slotToCard[i];
        if (card!=null) {
        deck.add(card);
        table.removeCard(i);
        }
    }
    }

    /**
     * Check who is/are the winner/s and displays them.
     */
    private void announceWinners() {
        // TODO implement
        
         int[] scores = new int[players.length];
         for (int i = 0; i < players.length; i++){
            scores[i]=players[i].score();
         }
        int max = 0;
         for (int i = 0; i < scores.length; i++) {
            if (scores[i]>max) {
                max=scores[i];
            }
        }
        int count=0;
        for (int i = 0; i < scores.length; i++) {
            if (scores[i]==max) {
                count++;
            }            
        }
        int[] winneres = new int[count];
        int j = 0 ;
        for (int i = 0; i < players.length; i++) {
           if(players[i].score()== max){
            winneres[j]= players[i].id;
            j++;
           }
            
            }   
        
   
        env.ui.announceWinner(winneres);
    }

    public void terminateall() {
        for (int i = players.length - 1; i >= 0; i--) {
            players[i].terminate();
        }
        terminate();
    }

    public boolean checksets() {
        // To Do Implement
        boolean ans = false;
        if (currentPlayer != null) {
            boolean fullSet = true;
            int[] slots = currentPlayer.cardsPressed();
            int[] cards = new int[slots.length];
                for (int i = 0; i < slots.length; i++) {
                    if (table.slotToCard[slots[i]] == null) {
                        currentPlayer.tokens.remove((Integer)slots[i]);
                        fullSet = false;
                        break;
                    }
                    cards[i]=table.slotToCard[slots[i]];
                }
            if (fullSet) {
                ans = env.util.testSet(cards);
                currentPlayer.canPlay = false;
                if (ans) {
                    currentPlayer.point = true;
                    currentPlayer.penalty = false;
                    for (Player p : players) {
                        for (int i = 0; i < cards.length; i++) {
                            currentPlayer.tokens.remove((Integer)slots[i]);
                            table.removeToken(p.id, slots[i]);                            
                        }
                    }
                    for (int i = 0; i < cards.length; i++) {
                        table.removeCard(slots[i]);
                    }
                 
                    updateTimerDisplay(true);
                } else if (!ans) {
                    currentPlayer.penalty = true;
                    currentPlayer.point = false;
                }
            } else {
                currentPlayer.canPlay = true;
            }
            currentPlayer.wakeUp();
            currentPlayer = null;
        }
        return ans;
    }

    public void addlegal(Player player) {
        try {
            submissions.add(player);
        } catch (Exception e) {
        }
    }
    
}
