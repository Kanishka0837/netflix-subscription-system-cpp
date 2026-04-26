// ============================================================
//  Netflix Subscription Management System
//  OOP using C++ — Console Based
//  Team: Grishma Bidkar, Kanishka Malpani,
//        Shruti Kuril, Utkarsha Patil
// ============================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include <algorithm>
using namespace std;

// ============================================================
//  CUSTOM EXCEPTIONS
// ============================================================
class InvalidPlanException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid subscription plan selected.";
    }
};
class AuthException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid email or password.";
    }
};
class DuplicateUserException : public exception {
public:
    const char* what() const noexcept override {
        return "User with this email already exists.";
    }
};

// ============================================================
//  REVIEW
// ============================================================
class Review {
private:
    string userEmail;
    string comment;
    int    rating;
public:
    Review(string email, string cmt, int r)
        : userEmail(email), comment(cmt), rating(r) {}
    string getEmail()   const { return userEmail; }
    string getComment() const { return comment;   }
    int    getRating()  const { return rating;    }
    friend ostream& operator<<(ostream& os, const Review& r) {
        os << "  [" << r.rating << "/5] " << r.comment << "  — " << r.userEmail;
        return os;
    }
};

// ============================================================
//  MOVIE
// ============================================================
class Movie {
private:
    int            id;
    string         title;
    string         genre;
    string         requiredPlan;
    int            watchCount;
    vector<Review> reviews;
public:
    Movie(int id, string title, string genre, string plan)
        : id(id), title(title), genre(genre), requiredPlan(plan), watchCount(0) {}

    int    getId()           const { return id;           }
    string getTitle()        const { return title;        }
    string getGenre()        const { return genre;        }
    string getRequiredPlan() const { return requiredPlan; }
    int    getWatchCount()   const { return watchCount;   }

    void incrementWatch() { watchCount++; }

    void addReview(const string& email, const string& comment, int rating) {
        for (auto& r : reviews) {
            if (r.getEmail() == email) {
                cout << "\n  You have already reviewed this movie." << endl;
                return;
            }
        }
        if (rating < 1 || rating > 5) {
            cout << "\n  Rating must be between 1 and 5." << endl;
            return;
        }
        reviews.emplace_back(email, comment, rating);
        cout << "\n  Review added successfully!" << endl;
    }

    double avgRating() const {
        if (reviews.empty()) return 0.0;
        double sum = 0;
        for (const auto& r : reviews) sum += r.getRating();
        return sum / reviews.size();
    }

    void showReviews() const {
        if (reviews.empty()) { cout << "\n  No reviews yet." << endl; return; }
        cout << "\n  --- Reviews for " << title << " ---" << endl;
        for (const auto& r : reviews) cout << r << endl;
        cout << "  Avg Rating: " << fixed << setprecision(1) << avgRating() << "/5" << endl;
    }

    void displayLine() const {
        cout << "  [" << setw(2) << id << "] "
             << left << setw(28) << title
             << setw(11) << genre
             << setw(10) << requiredPlan
             << "Watched: " << watchCount;
        if (!reviews.empty())
            cout << "  Avg: " << fixed << setprecision(1) << avgRating() << "/5";
        cout << endl;
    }

    bool operator>(const Movie& other) const { return watchCount > other.watchCount; }
};

// ============================================================
//  MOVIE CATALOGUE
// ============================================================
class MovieCatalogue {
private:
    vector<Movie> movies;
public:
    MovieCatalogue() {
        movies = {
            {1,  "Stranger Things",   "Sci-Fi",   "Basic"},
            {2,  "Money Heist",       "Thriller", "Basic"},
            {3,  "The Crown",         "Drama",    "Basic"},
            {4,  "Breaking Bad",      "Drama",    "Basic"},
            {5,  "Friends",           "Comedy",   "Basic"},
            {6,  "Wednesday",         "Horror",   "Standard"},
            {7,  "Squid Game",        "Thriller", "Standard"},
            {8,  "Dark",              "Sci-Fi",   "Standard"},
            {9,  "Ozark",             "Drama",    "Standard"},
            {10, "The Witcher",       "Fantasy",  "Standard"},
            {11, "The Last of Us",    "Action",   "Premium"},
            {12, "House of Cards",    "Drama",    "Premium"},
            {13, "Narcos",            "Crime",    "Premium"},
            {14, "Black Mirror",      "Sci-Fi",   "Premium"},
            {15, "Peaky Blinders",    "Crime",    "Premium"},
        };
    }

    vector<Movie*> getAccessibleMovies(const string& plan) {
        vector<Movie*> result;
        for (auto& m : movies) {
            if (plan == "Premium") result.push_back(&m);
            else if (plan == "Standard" && m.getRequiredPlan() != "Premium") result.push_back(&m);
            else if (plan == "Basic"    && m.getRequiredPlan() == "Basic")   result.push_back(&m);
        }
        return result;
    }

    void showTop10() const {
        vector<Movie> sorted = movies;
        sort(sorted.begin(), sorted.end(), [](const Movie& a, const Movie& b){
            return a.getWatchCount() > b.getWatchCount();
        });
        cout << "\n  --- Top 10 Most Watched ---" << endl;
        cout << "  " << left << setw(4) << "#" << setw(28) << "Title"
             << setw(11) << "Genre" << "Watched" << endl;
        cout << "  " << string(55, '-') << endl;
        int count = 0;
        for (const auto& m : sorted) {
            if (count >= 10 || m.getWatchCount() == 0) break;
            cout << "  " << setw(4) << (count+1) << left << setw(28) << m.getTitle()
                 << setw(11) << m.getGenre() << m.getWatchCount() << endl;
            count++;
        }
        if (count == 0) cout << "  No movies watched yet." << endl;
    }

    void showRecommendations(const string& plan) {
        vector<Movie*> accessible = getAccessibleMovies(plan);
        sort(accessible.begin(), accessible.end(), [](Movie* a, Movie* b){
            return a->avgRating() > b->avgRating();
        });
        cout << "\n  --- Recommended For You ---" << endl;
        cout << "  (Highest rated movies in your plan)" << endl;
        cout << "  " << left << setw(28) << "Title" << setw(11) << "Genre"
             << "Avg Rating" << endl;
        cout << "  " << string(52, '-') << endl;
        int count = 0;
        for (auto* m : accessible) {
            if (count >= 5) break;
            if (m->avgRating() == 0) continue;
            cout << "  " << left << setw(28) << m->getTitle()
                 << setw(11) << m->getGenre()
                 << fixed << setprecision(1) << m->avgRating() << "/5" << endl;
            count++;
        }
        if (count == 0) cout << "  No recommendations yet. Review some movies first!" << endl;
    }

    Movie* findById(int id) {
        for (auto& m : movies) if (m.getId() == id) return &m;
        return nullptr;
    }

    void showCatalogue(const string& plan) {
        vector<Movie*> accessible = getAccessibleMovies(plan);
        cout << "\n  --- Catalogue (" << plan << " plan) ---" << endl;
        cout << "  " << left << setw(5) << "ID" << setw(28) << "Title"
             << setw(11) << "Genre" << setw(10) << "Plan" << "Watched" << endl;
        cout << "  " << string(62, '-') << endl;
        for (auto* m : accessible) m->displayLine();
    }
};

// ============================================================
//  SUBSCRIPTION PLAN  (Abstract Base Class)
// ============================================================
class SubscriptionPlan {
protected:
    string planName;
    double price;
    int    screens;
    string quality;
public:
    SubscriptionPlan(string name, double p, int s, string q)
        : planName(name), price(p), screens(s), quality(q) {}
    virtual double getPrice()    const = 0;
    virtual void   displayInfo() const = 0;
    virtual string getPlanName() const { return planName; }
    virtual string getQuality()  const { return quality;  }
    virtual int    getScreens()  const { return screens;  }
    bool operator==(const SubscriptionPlan& o) const { return planName == o.planName; }
    bool operator<(const SubscriptionPlan& o)  const { return price < o.price; }
    friend ostream& operator<<(ostream& os, const SubscriptionPlan& p) {
        os << left << setw(12) << p.planName << "  Rs." << setw(6) << p.price
           << "  " << p.screens << " screen(s)  " << p.quality;
        return os;
    }
    virtual ~SubscriptionPlan() {}
};

class BasicPlan : public SubscriptionPlan {
public:
    BasicPlan() : SubscriptionPlan("Basic", 199.0, 1, "480p") {}
    double getPrice() const override { return price; }
    void displayInfo() const override {
        cout << "  Plan: " << planName << "  Price: Rs." << price
             << "  Screens: " << screens << "  Quality: " << quality << endl;
    }
};

class StandardPlan : public SubscriptionPlan {
public:
    StandardPlan() : SubscriptionPlan("Standard", 499.0, 2, "1080p") {}
    double getPrice() const override { return price; }
    void displayInfo() const override {
        cout << "  Plan: " << planName << "  Price: Rs." << price
             << "  Screens: " << screens << "  Quality: " << quality << endl;
    }
};

class PremiumPlan : public SubscriptionPlan {
public:
    PremiumPlan() : SubscriptionPlan("Premium", 799.0, 4, "4K Ultra HD") {}
    double getPrice() const override { return price; }
    void displayInfo() const override {
        cout << "  Plan: " << planName << "  Price: Rs." << price
             << "  Screens: " << screens << "  Quality: " << quality << endl;
    }
};

// ============================================================
//  PAYMENT
// ============================================================
class Payment {
private:
    string invoiceID, userEmail, date, planName;
    double amount;
public:
    static int invoiceCounter;
    Payment(string email, double amt, string plan) {
        invoiceCounter++;
        invoiceID = "INV" + to_string(1000 + invoiceCounter);
        userEmail = email; amount = amt; planName = plan;
        time_t now = time(0); char buf[20];
        strftime(buf, sizeof(buf), "%d-%m-%Y", localtime(&now));
        date = string(buf);
    }
    Payment(string id, string email, double amt, string plan, string d)
        : invoiceID(id), userEmail(email), amount(amt), planName(plan), date(d) {}
    string getEmail()     const { return userEmail; }
    string getInvoiceID() const { return invoiceID; }
    double getAmount()    const { return amount;    }
    string getPlanName()  const { return planName;  }
    string getDate()      const { return date;      }
    friend ostream& operator<<(ostream& os, const Payment& p) {
        os << p.invoiceID << " | " << p.date << " | " << p.planName << " | Rs." << p.amount;
        return os;
    }
};
int Payment::invoiceCounter = 0;

// ============================================================
//  USER  (Base Class)
// ============================================================
class User {
private:
    string password;
protected:
    string name, email, role;
public:
    static int userCount;
    User(string n, string e, string p, string r = "user")
        : name(n), email(e), password(p), role(r) { userCount++; }
    virtual ~User() { userCount--; }
    string getName()  const { return name;  }
    string getEmail() const { return email; }
    string getRole()  const { return role;  }
    bool verifyPassword(const string& input) const { return input == password; }
    string getPassword() const { return password; }
    virtual void showDashboard() {
        cout << "\n  Logged in as: " << name << " (" << email << ")" << endl;
    }
    bool operator==(const User& other) const { return email == other.email; }
    friend ostream& operator<<(ostream& os, const User& u) {
        os << left << setw(20) << u.name << setw(30) << u.email << setw(10) << u.role;
        return os;
    }
};
int User::userCount = 0;

// ============================================================
//  SUBSCRIBER  (inherits User)
// ============================================================
class Subscriber : public User {
private:
    string          planName;
    string          status;
    vector<Payment> paymentHistory;
public:
    Subscriber(string n, string e, string p)
        : User(n, e, p, "user"), planName("None"), status("None") {}
    Subscriber(string n, string e, string p, string plan, string stat)
        : User(n, e, p, "user"), planName(plan), status(stat) {}

    string getPlanName() const { return planName; }
    string getStatus()   const { return status;   }
    void addPayment(const Payment& pay) { paymentHistory.push_back(pay); }
    const vector<Payment>& getPayments() const { return paymentHistory; }

    void subscribe(SubscriptionPlan* plan) {
        planName = plan->getPlanName(); status = "Active";
        paymentHistory.emplace_back(email, plan->getPrice(), planName);
        cout << "\n  Subscribed to " << planName << " successfully!" << endl;
    }
    void changePlan(SubscriptionPlan* newPlan) {
        string old = planName;
        planName = newPlan->getPlanName(); status = "Active";
        paymentHistory.emplace_back(email, newPlan->getPrice(), planName);
        cout << "\n  Plan changed from " << old << " to " << planName << "!" << endl;
    }
    void cancelSubscription() {
        if (status == "None" || status == "Cancelled") {
            cout << "\n  No active subscription to cancel." << endl; return;
        }
        status = "Cancelled"; planName = "None";
        cout << "\n  Subscription cancelled." << endl;
    }
    void showDashboard() override {
        cout << "\n  ----------------------------------------" << endl;
        cout << "  Welcome, " << name   << endl;
        cout << "  Email  : "  << email   << endl;
        cout << "  Plan   : "  << planName << endl;
        cout << "  Status : "  << status  << endl;
        cout << "  ----------------------------------------" << endl;
    }
    void viewBillingHistory() const {
        if (paymentHistory.empty()) { cout << "\n  No payment history." << endl; return; }
        cout << "\n  --- Billing History ---" << endl;
        for (const auto& p : paymentHistory) cout << "  " << p << endl;
    }
    double totalSpent() const {
        double total = 0;
        for (const auto& p : paymentHistory) total += p.getAmount();
        return total;
    }
};

// ============================================================
//  ADMIN  (inherits User)
// ============================================================
class Admin : public User {
public:
    Admin(string n, string e, string p) : User(n, e, p, "admin") {}
    void showDashboard() override {
        cout << "\n  ----------------------------------------" << endl;
        cout << "  Admin Panel — " << name << endl;
        cout << "  ----------------------------------------" << endl;
    }
    void viewAllUsers(const vector<Subscriber>& subs) const {
        if (subs.empty()) { cout << "\n  No users." << endl; return; }
        cout << "\n  " << left << setw(20) << "Name" << setw(30) << "Email"
             << setw(12) << "Plan" << "Status" << endl;
        cout << "  " << string(72, '-') << endl;
        for (const auto& s : subs)
            cout << "  " << setw(20) << s.getName() << setw(30) << s.getEmail()
                 << setw(12) << s.getPlanName() << s.getStatus() << endl;
    }
    void viewRevenueReport(const vector<Subscriber>& subs) const {
        double total = 0; int active = 0;
        map<string, int> planCount;
        for (const auto& s : subs) {
            total += s.totalSpent();
            if (s.getStatus() == "Active") { active++; planCount[s.getPlanName()]++; }
        }
        cout << "\n  Total Revenue : Rs." << total << endl;
        cout << "  Active Users  : " << active << endl;
        cout << "  Total Users   : " << subs.size() << endl;
        for (auto& kv : planCount)
            cout << "  " << kv.first << " : " << kv.second << " user(s)" << endl;
    }
    void searchUser(const vector<Subscriber>& subs, const string& q) const {
        bool found = false;
        for (const auto& s : subs) {
            if (s.getEmail() == q || s.getName() == q) {
                cout << "  Name: " << s.getName() << "  Email: " << s.getEmail()
                     << "  Plan: " << s.getPlanName() << "  Status: " << s.getStatus() << endl;
                found = true;
            }
        }
        if (!found) cout << "\n  No user found." << endl;
    }
    void deleteUserSubscription(vector<Subscriber>& subs, const string& email) {
        for (auto& s : subs) {
            if (s.getEmail() == email) {
                if (s.getStatus() == "None" || s.getStatus() == "Cancelled") {
                    cout << "\n  No active subscription." << endl; return;
                }
                s.cancelSubscription();
                return;
            }
        }
        cout << "\n  User not found." << endl;
    }
    void updateUserSubscription(vector<Subscriber>& subs,
                                const string& email, SubscriptionPlan* plan) {
        for (auto& s : subs) {
            if (s.getEmail() == email) {
                if (s.getStatus() != "Active") {
                    cout << "\n  User has no active subscription." << endl; return;
                }
                s.changePlan(plan);
                return;
            }
        }
        cout << "\n  User not found." << endl;
    }
};

// ============================================================
//  FILE HANDLER
// ============================================================
class FileHandler {
private:
    string usersFile    = "users.txt";
    string paymentsFile = "payments.txt";
public:
    void saveUsers(const vector<Subscriber>& subs) {
        ofstream f(usersFile);
        for (const auto& s : subs)
            f << s.getName() << "|" << s.getEmail() << "|" << s.getPassword()
              << "|" << s.getPlanName() << "|" << s.getStatus() << "\n";
        f.close();
    }
    vector<Subscriber> loadUsers() {
        vector<Subscriber> subs;
        ifstream f(usersFile);
        if (!f.is_open()) return subs;
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            vector<string> parts; string token;
            for (char c : line) {
                if (c == '|') { parts.push_back(token); token = ""; } else token += c;
            }
            parts.push_back(token);
            if (parts.size() == 5)
                subs.emplace_back(parts[0], parts[1], parts[2], parts[3], parts[4]);
        }
        f.close();
        return subs;
    }
    void savePayment(const Payment& p) {
        ofstream f(paymentsFile, ios::app);
        f << p.getInvoiceID() << "|" << p.getEmail() << "|" << p.getAmount()
          << "|" << p.getPlanName() << "|" << p.getDate() << "\n";
        f.close();
    }
    vector<Payment> loadPayments(const string& email) {
        vector<Payment> payments;
        ifstream f(paymentsFile);
        if (!f.is_open()) return payments;
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            vector<string> parts; string token;
            for (char c : line) {
                if (c == '|') { parts.push_back(token); token = ""; } else token += c;
            }
            parts.push_back(token);
            if (parts.size() == 5 && parts[1] == email)
                payments.emplace_back(parts[0], parts[1], stod(parts[2]), parts[3], parts[4]);
        }
        f.close();
        return payments;
    }
};

// ============================================================
//  HELPERS
// ============================================================
SubscriptionPlan* getPlan(int choice) {
    switch (choice) {
        case 1: return new BasicPlan();
        case 2: return new StandardPlan();
        case 3: return new PremiumPlan();
        default: throw InvalidPlanException();
    }
}
void showAvailablePlans() {
    cout << "\n  Available Plans:" << endl;
    cout << "  1. "; BasicPlan b;    cout << b << endl;
    cout << "  2. "; StandardPlan s; cout << s << endl;
    cout << "  3. "; PremiumPlan p;  cout << p << endl;
}
void separator() { cout << "\n  ========================================" << endl; }

// ============================================================
//  MOVIE MENU
// ============================================================
void movieMenu(Subscriber& user, MovieCatalogue& catalogue) {
    if (user.getStatus() != "Active") {
        cout << "\n  You need an active subscription to watch movies." << endl;
        return;
    }
    string plan = user.getPlanName();
    int choice;
    while (true) {
        cout << "\n  --- Movies & Shows ---"       << endl;
        cout << "  1. Browse catalogue"            << endl;
        cout << "  2. Watch a movie"               << endl;
        cout << "  3. Top 10 most watched"         << endl;
        cout << "  4. Recommended for you"         << endl;
        cout << "  5. Add a review"                << endl;
        cout << "  6. View reviews for a movie"    << endl;
        cout << "  7. Back"                        << endl;
        cout << "\n  Enter choice: ";
        cin  >> choice;

        if (choice == 1) {
            separator();
            catalogue.showCatalogue(plan);

        } else if (choice == 2) {
            separator();
            catalogue.showCatalogue(plan);
            cout << "\n  Enter movie ID to watch: ";
            int id; cin >> id;
            Movie* m = catalogue.findById(id);
            if (!m) { cout << "\n  Movie not found." << endl; continue; }
            bool allowed = (plan == "Premium") ||
                           (plan == "Standard" && m->getRequiredPlan() != "Premium") ||
                           (plan == "Basic"    && m->getRequiredPlan() == "Basic");
            if (!allowed)
                cout << "\n  Requires " << m->getRequiredPlan()
                     << " plan or higher. Please upgrade." << endl;
            else {
                m->incrementWatch();
                cout << "\n  Now playing: " << m->getTitle()
                     << " (" << m->getGenre() << ")  Enjoy!" << endl;
            }

        } else if (choice == 3) {
            separator();
            catalogue.showTop10();

        } else if (choice == 4) {
            separator();
            catalogue.showRecommendations(plan);

        } else if (choice == 5) {
            separator();
            catalogue.showCatalogue(plan);
            cout << "\n  Enter movie ID to review: ";
            int id; cin >> id;
            Movie* m = catalogue.findById(id);
            if (!m) { cout << "\n  Movie not found." << endl; continue; }
            cout << "  Rating (1-5): ";
            int rating; cin >> rating;
            cin.ignore();
            cout << "  Comment: ";
            string comment; getline(cin, comment);
            m->addReview(user.getEmail(), comment, rating);

        } else if (choice == 6) {
            separator();
            catalogue.showCatalogue(plan);
            cout << "\n  Enter movie ID: ";
            int id; cin >> id;
            Movie* m = catalogue.findById(id);
            if (!m) cout << "\n  Movie not found." << endl;
            else     m->showReviews();

        } else if (choice == 7) {
            break;
        } else {
            cout << "\n  Invalid choice." << endl;
        }
    }
}

// ============================================================
//  USER MENU
// ============================================================
void userMenu(Subscriber& user, FileHandler& fh, MovieCatalogue& catalogue) {
    vector<Payment> savedPays = fh.loadPayments(user.getEmail());
    for (auto& p : savedPays) user.addPayment(p);

    int choice;
    while (true) {
        user.showDashboard();
        cout << "\n  1. View subscription details" << endl;
        cout << "  2. Subscribe to a plan"        << endl;
        cout << "  3. Upgrade / Change plan"       << endl;
        cout << "  4. Cancel subscription"         << endl;
        cout << "  5. View billing history"        << endl;
        cout << "  6. Movies & Shows"              << endl;
        cout << "  7. Logout"                      << endl;
        cout << "\n  Enter choice: ";
        cin  >> choice;

        if (choice == 1) {
            separator();
            if (user.getStatus() == "None" || user.getStatus() == "Cancelled")
                cout << "\n  No active subscription." << endl;
            else
                cout << "\n  Plan: " << user.getPlanName()
                     << "  Status: " << user.getStatus() << endl;

        } else if (choice == 2) {
            if (user.getStatus() == "Active") {
                cout << "\n  Already subscribed. Use option 3 to change." << endl; continue;
            }
            separator(); showAvailablePlans();
            cout << "\n  Select plan (1-3): ";
            int pc; cin >> pc;
            try {
                SubscriptionPlan* plan = getPlan(pc);
                user.subscribe(plan);
                const auto& pays = user.getPayments();
                if (!pays.empty()) fh.savePayment(pays.back());
                fh.saveUsers({user});
                delete plan;
            } catch (InvalidPlanException& e) { cout << "\n  " << e.what() << endl; }

        } else if (choice == 3) {
            if (user.getStatus() != "Active") {
                cout << "\n  No active subscription. Subscribe first." << endl; continue;
            }
            separator(); showAvailablePlans();
            cout << "\n  Select new plan (1-3): ";
            int pc; cin >> pc;
            try {
                SubscriptionPlan* plan = getPlan(pc);
                user.changePlan(plan);
                const auto& pays = user.getPayments();
                if (!pays.empty()) fh.savePayment(pays.back());
                fh.saveUsers({user});
                delete plan;
            } catch (InvalidPlanException& e) { cout << "\n  " << e.what() << endl; }

        } else if (choice == 4) {
            user.cancelSubscription();
            fh.saveUsers({user});

        } else if (choice == 5) {
            separator();
            user.viewBillingHistory();

        } else if (choice == 6) {
            separator();
            movieMenu(user, catalogue);

        } else if (choice == 7) {
            cout << "\n  Logged out. Goodbye, " << user.getName() << "!" << endl;
            break;
        } else {
            cout << "\n  Invalid choice." << endl;
        }
    }
}

// ============================================================
//  ADMIN MENU
// ============================================================
void adminMenu(Admin& admin, vector<Subscriber>& subs, FileHandler& fh) {
    admin.showDashboard();
    int choice;
    while (true) {
        cout << "\n  1. View all users"              << endl;
        cout << "  2. Revenue report"               << endl;
        cout << "  3. Search user"                  << endl;
        cout << "  4. Cancel a user's subscription" << endl;
        cout << "  5. Update a user's plan"         << endl;
        cout << "  6. Logout"                       << endl;
        cout << "\n  Enter choice: ";
        cin  >> choice;

        if (choice == 1) {
            separator(); admin.viewAllUsers(subs);
        } else if (choice == 2) {
            separator(); admin.viewRevenueReport(subs);
        } else if (choice == 3) {
            separator();
            cout << "\n  Enter email or name: "; string q; cin >> q;
            admin.searchUser(subs, q);
        } else if (choice == 4) {
            separator();
            cout << "\n  Enter user email: "; string email; cin >> email;
            admin.deleteUserSubscription(subs, email);
            fh.saveUsers(subs);
        } else if (choice == 5) {
            separator();
            cout << "\n  Enter user email: "; string email; cin >> email;
            showAvailablePlans();
            cout << "\n  Select new plan (1-3): "; int pc; cin >> pc;
            try {
                SubscriptionPlan* plan = getPlan(pc);
                admin.updateUserSubscription(subs, email, plan);
                // BILLING FIX: save payment after admin update
                for (auto& s : subs) {
                    if (s.getEmail() == email) {
                        const auto& pays = s.getPayments();
                        if (!pays.empty()) fh.savePayment(pays.back());
                        break;
                    }
                }
                fh.saveUsers(subs);
                delete plan;
            } catch (InvalidPlanException& e) { cout << "\n  " << e.what() << endl; }
        } else if (choice == 6) {
            cout << "\n  Admin logged out." << endl; break;
        } else {
            cout << "\n  Invalid choice." << endl;
        }
    }
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    FileHandler        fh;
    MovieCatalogue     catalogue;
    vector<Subscriber> subscribers = fh.loadUsers();
    Admin admin("Admin", "admin@netflix.com", "admin123");

    cout << "\n  ========================================" << endl;
    cout << "    Netflix Subscription Management System" << endl;
    cout << "  ========================================" << endl;

    int choice;
    while (true) {
        cout << "\n  1. Register" << endl;
        cout << "  2. Login"     << endl;
        cout << "  3. Exit"      << endl;
        cout << "\n  Enter choice: ";
        cin  >> choice;

        if (choice == 1) {
            separator();
            string name, email, pass;
            cout << "\n  Enter name    : "; cin >> ws; getline(cin, name);
            cout << "  Enter email   : "; cin >> email;
            cout << "  Enter password: "; cin >> pass;
            try {
                for (auto& s : subscribers)
                    if (s.getEmail() == email) throw DuplicateUserException();
                subscribers.emplace_back(name, email, pass);
                fh.saveUsers(subscribers);
                cout << "\n  Registration successful!" << endl;
            } catch (DuplicateUserException& e) { cout << "\n  " << e.what() << endl; }

        } else if (choice == 2) {
            separator();
            string email, pass;
            cout << "\n  Enter email   : "; cin >> email;
            cout << "  Enter password: "; cin >> pass;
            try {
                if (email == admin.getEmail() && admin.verifyPassword(pass)) {
                    adminMenu(admin, subscribers, fh); continue;
                }
                bool found = false;
                for (auto& s : subscribers) {
                    if (s.getEmail() == email) {
                        if (!s.verifyPassword(pass)) throw AuthException();
                        found = true;
                        userMenu(s, fh, catalogue);
                        fh.saveUsers(subscribers);
                        break;
                    }
                }
                if (!found) throw AuthException();
            } catch (AuthException& e) { cout << "\n  " << e.what() << endl; }

        } else if (choice == 3) {
            cout << "\n  Goodbye!\n" << endl; break;
        } else {
            cout << "\n  Invalid choice." << endl;
        }
    }
    return 0;
}