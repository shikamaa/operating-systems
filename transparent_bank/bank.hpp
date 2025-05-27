#include <iostream>
#include <stdexcept>
#include <array>
#include <climits>

class BankCell {
private:
    int current_balance;
    int minimal_balance;
    int maximal_balance;
    bool is_frozen;

public:
    BankCell()
        : current_balance(0), minimal_balance(0),
          maximal_balance(INT_MAX), is_frozen(false) {}

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

    bool deposit(int amount) {
        if (is_frozen) {
            std::cerr << "[Error] This cell is frozen\n";
            return false;
        }
        if (amount < 0) {
            std::cerr << "[Error] Deposit amount cannot be negative\n";
            return false;
        }
        if (current_balance > maximal_balance - amount) {
            std::cerr << "[Error] Deposit exceeds maximum allowed balance\n";
            return false;
        }
        current_balance += amount;
        return true;
    }

    bool withdraw(int amount) {
        if (is_frozen) {
            std::cerr << "[Error] Cell is frozen\n";
            return false;
        }
        if (amount < 0) {
            std::cerr << "[Error] Withdrawal amount cannot be negative\n";
            return false;
        }
        if (current_balance - amount < minimal_balance) {
            std::cerr << "[Error] Withdrawal would go below minimal balance\n";
            return false;
        }
        current_balance -= amount;
        return true;
    }
};

class Bank {
private:
    std::array<BankCell, 100> accounts;

    void check_index(int index) const {
        if (index < 0 || index >= static_cast<int>(accounts.size())) {
            throw std::out_of_range("Invalid account index");
        }
    }

public:
    Bank() = default;

    BankCell& get_account(int index) {
        check_index(index);
        return accounts[index];
    }

    int total_balance() const {
        int total_sum = 0;
        for (const auto& acc : accounts) {
            total_sum += acc.get_balance();
        }
        return total_sum;
    }

    void freeze_account(int index) {
        check_index(index);
        accounts[index].freeze();
    }

    void unfreeze_account(int index) {
        check_index(index);
        accounts[index].unfreeze();
    }

    bool transfer(int from, int to, int amount) {
        try {
            check_index(from);
            check_index(to);
        } catch (const std::out_of_range& e) {
            std::cerr << "[Error] " << e.what() << "\n";
            return false;
        }

        BankCell& sender = accounts[from];
        BankCell& receiver = accounts[to];

        if (sender.get_frozen_status()) {
            std::cerr << "[Error] Sender account is frozen\n";
            return false;
        }

        if (receiver.get_frozen_status()) {
            std::cerr << "[Error] Receiver account is frozen\n";
            return false;
        }

        if (!sender.withdraw(amount)) {
            return false;
        }

        if (!receiver.deposit(amount)) {
            sender.deposit(amount);
            std::cerr << "[Error] Deposit failed, rolling back transfer\n";
            return false;
        }
        return true;
    }
    int size() const {
        return static_cast<int>(accounts.size());
    }
};
