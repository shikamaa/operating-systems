#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <array>

class BankCell {
    private:
    int current_balance;
    int minimal_balance;
    int maximal_balance;
    bool is_frozen;
    public:
    BankCell() : current_balance(0), minimal_balance(0), maximal_balance(0), is_frozen(false) {}

    bool get_frozen_status() const {
        return is_frozen;
    }

    int get_balance() const {
        return current_balance;
    }
    int get_min_balance() const {
        return minimal_balance;
    }
    int get_max_balance() const {
        return maximal_balance;
    }
    void freeze() {
        is_frozen = true;
    }
    void unfreeze() {
        is_frozen = false;
    }
    void deposit(int amount) {
        if (is_frozen) {
            perror("This cell is frozen \n");
        }
        current_balance+= amount;
    }
    bool withdraw(int amount) {
        if (is_frozen == true) {
            perror("This bank cell is frozen \n");
            return false;
        }
        if (amount > current_balance) {
            perror("Current amount is greater than current balance. You can't withdraw your money \n");
            return false;
        }
        current_balance -= amount;
        return true;
    }
};

class Bank {
    private:
    std::array<BankCell, 100> accounts;
    public:
    Bank() {
        for (int i = 0; i < accounts.size(); ++i)
            accounts[i] = BankCell();
    }
    BankCell& get_account(int index) {
        if (index < 0 || index >= accounts.size()) {
            perror("Invalid account index \n");
            exit(1);
        }
        return accounts[index];
    }
    int total_balance() {
        int total_sum = 0;
        for (int i = 0; i < accounts.size(); ++i)
            total_sum += accounts[i].get_balance();
        return total_sum;
    }
    void freeze_account(int index) {
        accounts[index].freeze();
    }
    void unfreeze_account(int index) {
        accounts[index].unfreeze();
    }

    bool transfer(int from, int to, int amount) {
        if (from < 0 || from >= accounts.size() || (to < 0 || to >= accounts.size())) {
            perror("Invalid account index \n");
            return false;
        }
        if (accounts[from].get_frozen_status()) {
            std::cerr << "This bank cell is frozen \n";
            return false;
        }

        if (accounts[to].get_frozen_status()) {
            std::cerr << "This bank cell is frozen \n";
            return false;
        }

        BankCell& from_account = accounts[from];
        BankCell& to_account = accounts[to];
        
        if (!from_account.withdraw(amount)) 
            return false;
        
        to_account.deposit(amount);
        return true;
    }
};