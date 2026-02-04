#include <iostream>
#include <fstream>
#include <thread>
#include <memory>
#include <vector>
#include <array>
#include <cstring>
#include <optional>
#include <utility>
#include <chrono>
#include <future>
#include <filesystem>
#include <random>
#include <algorithm>

namespace fs = std::filesystem;

struct player_stats{
    int id;
    std::string name;
    int money;
    std::vector<std::array<char,2>> hand;

    player_stats(int val,std::string txt,int mon) : id(val), name(std::move(txt)),money(mon) {}
};

struct player_move{
    int id;
    std::array<char,2> card_played;
    int bet;

    player_move(int val, std::array<char,2> card, int mon) : id(val), card_played(card),bet(mon){}
};

std::mutex table_mtx;
struct game_table{
    std::vector<std::unique_ptr<player_move>> moves;
    int winner;
    int winnings;
};
game_table table;

std::vector<std::thread> players;
std::vector<std::shared_ptr<player_stats>> leaderboard;

std::vector<std::array<char,2>> deck;
std::vector<std::array<char,2>> used_deck;


std::mutex output_mtx; //Used to prevent threads from being interrupted while printing 

std::mutex check_winner_mtx;
std::condition_variable check_winner_cnd;

std::condition_variable wait_result_cnd;

std::mutex turn_mtx;
std::condition_variable turn_cnd;

std::atomic<int> total_players=0;
std::atomic<int> waiting=0;

std::atomic<bool> cards_ready=false;
std::atomic<bool> is_dealer_allowed=false;
std::atomic<bool> results_ready=false;

bool automatic=false;

std::condition_variable dealer_turn_cnd;

std::condition_variable all_ready_cnd;


std::ostream& operator <<(std::ostream& os, const std::array<char,2>& card){ 
    os << card[0] << card[1];
    return os;
}

int random_num_gen(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(gen);
}

void player_thread(std::shared_ptr<player_stats> player_info){
    std::unique_lock<std::mutex> out_lock(output_mtx);
    std::cout<< "Hello my name is "<< player_info->name << " and I have " << player_info->money <<'$'<<std::endl;
    out_lock.unlock();
    std::unique_lock<std::mutex> lock(turn_mtx);
    waiting--;
    if(!waiting)
        dealer_turn_cnd.notify_one();
    while(player_info->money){

        turn_cnd.wait(lock,[](){return cards_ready || total_players==1;});

        if(total_players==1)
            break;
        out_lock.lock();
        std::cout <<player_info->name << ": My cards are ";
        for(auto card : player_info->hand)
            std::cout<< card <<' ';
        std::cout<< std::endl;
        out_lock.unlock();
        std::unique_lock<std::mutex>table_lock(table_mtx);
        if(!player_info->hand.empty()){
            int rand_index=random_num_gen(0,(player_info->hand).size()-1);
            auto chosen_card = std::move(player_info->hand[rand_index]);
            int my_bet = random_num_gen(0,player_info->money);
            out_lock.lock();
            std::cout<< player_info->name<<": I bet "<< my_bet << " and I played "<< chosen_card<< std::endl;
            out_lock.unlock();
            auto my_move = std::make_unique<player_move>(player_info->id,chosen_card,my_bet);
            table.moves.push_back(std::move(my_move));
            table.winnings+=my_bet;
            player_info->money-=my_bet;
            player_info->hand.erase(player_info->hand.begin() + rand_index);
        }
        table_lock.unlock();
        waiting--;
        if(!waiting)
            dealer_turn_cnd.notify_one();
        wait_result_cnd.wait(lock,[](){return results_ready.load();});
        out_lock.lock();
         std::cout <<player_info->name <<": I ";
        if(player_info->id == table.winner)
            std::cout <<"won";
        else
            std::cout <<"lost";
        std::cout <<" and now I have "<< player_info->money <<'$' <<std::endl;
        out_lock.unlock();
        if(total_players!=1){
            if(!player_info->money)
                total_players--;
            else
                waiting--;
            if(abs(waiting) == total_players)
                all_ready_cnd.notify_one();
        }
    }
    if(total_players==1){
        all_ready_cnd.notify_one();
    }
    lock.unlock();
    out_lock.lock();
    if(!player_info->money)
        std::cout<< player_info->name<<": I lost all my money and I left the game..."<<std::endl;
    else
        std::cout<< player_info->name<<": I WON!! Total winnings: "<<player_info->money<<std::endl;
    out_lock.unlock();
}
std::array<char,2> get_card(){
    if(deck.empty()){
        deck=std::move(used_deck);
        used_deck.clear();
        std::shuffle(deck.begin(),deck.end(),std::random_device {});
    }
    auto card = std::move(deck.back());
    deck.pop_back();
    used_deck.push_back(card);
    return card;
}

void deck_init(){
    constexpr std::array seeds ={'H','S','C','D'};
    deck.reserve(40);
    used_deck.reserve(40);
    for(char seed : seeds){
        for(char j='0';j<='9';j++){
            used_deck.emplace_back(std::array{j, seed}); // the deck will be shuffled by the first get_card() call
        }
    }
}

void dealer_thread(std::string& winner){
    deck_init();
    std::unique_lock<std::mutex> out_lock(output_mtx);
    std::cout<<"The dealer is ready" << std::endl;
    out_lock.unlock();

    std::unique_lock<std::mutex> lock(turn_mtx);
    while(1){
        dealer_turn_cnd.wait(lock,[](){return (!waiting && is_dealer_allowed) || total_players==1;});
        if(total_players==1)
            break;
        is_dealer_allowed=false;
        table.winnings=0;
        for (auto& player : leaderboard){
            if(player->hand.empty()){
                for(int i=0;i<4;i++)
                    player->hand.push_back(get_card());
            }
        }
        waiting= total_players.load();
        cards_ready=true;
        results_ready=false;
        turn_cnd.notify_all();
        dealer_turn_cnd.wait(lock,[](){return !waiting;});
        auto target_card=get_card();
        out_lock.lock();
        std::cout <<std::endl<<"Bets have been placed"<< std::endl;
        std::cout << "Total win: " << table.winnings<<std::endl;
        std::cout<< "Target card: " << target_card << std::endl;
        out_lock.unlock();
        auto winning_guess= std::make_unique<player_move>(-1,std::array<char,2>{-1,-1},0);
        std::unique_lock<std::mutex>table_lock(table_mtx);

        for(auto& guess : table.moves){
            int guess_distance=abs(guess->card_played[0]-target_card[0]);
            int winning_distance=abs(winning_guess->card_played[0]-target_card[0]);
            if(guess_distance < winning_distance){
                if(guess_distance==winning_distance){
                    if(abs(guess->card_played[0]-target_card[0]) < abs(winning_guess->card_played[0]-target_card[0]))
                        winning_guess=std::move(guess);    
                }
                else{
                    winning_guess=std::move(guess);
                }
            }
        }
        table.moves.clear();
        std::unique_lock<std::mutex> winner_lock(check_winner_mtx);
        if(winning_guess->id != -1){
            leaderboard[winning_guess->id]->money += table.winnings;
            table.winner=winning_guess->id;
            std::string winner_name = leaderboard[winning_guess->id]->name;
            winner=winner_name;
        }else{
           winner="";
        }
        table_lock.unlock();
        out_lock.lock();
        std::cout<<"Dealer is waiting"<<std::endl;
        out_lock.unlock();
        check_winner_cnd.notify_one();
        winner_lock.unlock();
        cards_ready=false;
        results_ready=true;
        wait_result_cnd.notify_all();
    }
    lock.unlock();
    out_lock.lock();
    std::cout<< "Dealer out"<<std::endl;
    out_lock.unlock();
}

int main(int argn,char*argv[]){
    if (argn > 1){
        if(!strcmp("-a",argv[1])){
            automatic=true;
            std::cout<<"Automatic mode activated"<<std::endl;
        }
        else
            std::cout<<"Use  \"./card_game -a\" for automatic mode"<<std::endl; 
    }

    std::string winner_name;

    std::vector<std::string> name_list;
    std::vector<int> bank_balance;
    std::string temp_balance;

    fs::path player_folder= "players";

    if(fs::exists(player_folder) && fs::is_directory(player_folder)){
        try{
            for(auto f : fs::directory_iterator(player_folder)){
                std::ifstream file(f.path());
                if(!file.is_open()){
                    std::cerr << "Unable to open file "<< f.path()<< std::endl;
                    return 0;
                }
                file >> temp_balance;
                try{
                    bank_balance.push_back(std::stoi(temp_balance));
                } catch (const std::invalid_argument& e){
                    std::cerr <<"Bad file input: "<<f.path() <<std::endl;
                } catch (const std::out_of_range& e){
                    std::cerr <<"Bad file input: "<<f.path() <<std::endl;
                } 
                name_list.push_back(f.path().filename());
            }
        } catch ( const fs::filesystem_error& e){
            std::cerr<< "Errore: "<< e.what() << std::endl;
        }
    }
    else{
        std::cerr <<"Folder "<< player_folder <<" does not exist"<< std::endl;
        return 0;
    }
    
    for (auto&& elem : name_list){
        leaderboard.push_back(std::make_shared<player_stats>(total_players,std::move(elem),bank_balance[total_players]));
        total_players++;
    }

    waiting=total_players.load();

    name_list.clear();
    name_list.shrink_to_fit();

    bank_balance.clear();
    bank_balance.shrink_to_fit();
    
    for(auto p_info : leaderboard){
       players.emplace_back(player_thread,p_info);
    }
    std::unique_lock<std::mutex> out_lock(output_mtx);
    out_lock.unlock();
    std::thread dealer(dealer_thread,std::ref(winner_name));
    while(1){
        std::cout <<"The dealer can start the round"<<std::endl;
        is_dealer_allowed=true;
        dealer_turn_cnd.notify_one();
        try { 
            std::unique_lock<std::mutex> winner_lock(check_winner_mtx);
            check_winner_cnd.wait(winner_lock,[&](){return !winner_name.empty();});
            out_lock.lock();
            if(winner_name.empty()){
                std::cout << "No winners this round... ";
            }
            std::cout << "The winner of this round is " << winner_name<<'!';
            std::cout << " Waiting for players to be ready..."<< std::endl;
            out_lock.unlock();
            std::unique_lock<std::mutex> lock(turn_mtx);
            all_ready_cnd.wait(lock,[](){return abs(waiting)==total_players || total_players==1;});
            waiting=0;
            
            if(total_players==1){
                turn_cnd.notify_all();
                dealer_turn_cnd.notify_one();
                out_lock.lock();
                std::cout<<winner_name <<" defeated everyone and won the game!"<<std::endl;
                out_lock.unlock();
                lock.unlock();
                break;
            }
            winner_name.clear();
            winner_lock.unlock();
            lock.unlock();
            out_lock.lock();
            if(automatic){
                std::cout << "Next game in " << 5 << " seconds..."<<std::endl;
                out_lock.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(5));
                out_lock.lock();
            }else{
                std::cout <<std::endl<< "Press enter to start the next game..." << std::endl;
                std::cin.get();
            }
            std::cout<<std::endl<<"-----------------------------------------------"<<std::endl;
            std::cout<<std::endl<<"Starting new game with " <<total_players << " players"<<std::endl;
            out_lock.unlock();
        } catch(const std::exception& err){
            std::cerr << "Error: " << err.what()<< std::endl;
            break;
        }
    }
    for(auto& son : players)
        son.join();
    dealer.join();
    return 0;
}