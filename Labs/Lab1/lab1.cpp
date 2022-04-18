/*******************************************************
PROGRAM NAME - Lab 1 - Double Linked List
PROGRAMMER - Nathan Jaggers
DATE - 03/31/22
DESCRIPTION - This program acts as a review  exercise
              of pointers and double linked list.
*******************************************************/

#include <iostream>
#include <string.h>

#define SIZE 1000

using namespace std;

struct ListElement
{
    ListElement *next, *prev;
    char text[SIZE];
};

class DoubleLinkedList
{
private:
    ListElement *head;

public:
    DoubleLinkedList();
    void addNode();
    void printList();
    void deleteNode();
    ListElement *findTail();
};

int main()
{
    // declarations
    int choice = 0;
    DoubleLinkedList list1;

    // run menu and operations till quit
    while (choice != 4)
    {
        cout << "\nPlease make a selection:\n";
        cout << "1 push string\n"
             << "2 print list \n"
             << "3 delete item\n"
             << "4 end program\n"
             << ">>";
        cin >> choice;

        switch (choice)
        {
        case 1:
            list1.addNode();
            break;
        case 2:
            list1.printList();
            break;
        case 3:
            list1.deleteNode();
            break;
        case 4:
            cout << "Exiting... Have a nice day!\n";
            break;
        default:
            cout << "***Error***\nNot a valid selection. Try again.\n";
            break;
        }
    }

    return 0;
}

DoubleLinkedList ::DoubleLinkedList()
{
    head = NULL;
}
void DoubleLinkedList ::addNode()
{
    // create new list element
    ListElement *newNode = new ListElement;
    cout << "Text to add to list:";
    cin.ignore();
    cin >> newNode->text;
    newNode->next = NULL;
    newNode->prev = NULL;

    // test list and add new node to end
    if (head == NULL)
    {
        // make new entry head of list
        this->head = newNode;
    }
    else
    {
        // put new entry at end of list
        ListElement *tail = this->findTail();
        tail->next = newNode;
        newNode->prev = tail;
    }
}
void DoubleLinkedList ::printList()
{
    // go through nodes and print contents
    for (ListElement *i = this->head; i != NULL; i = i->next)
    {
        cout << i->text << endl;
    }
}
void DoubleLinkedList ::deleteNode()
{
    // get user choice on node to delete
    int temp = 0, counter = 1;
    cout << "Which index for node to delete:";
    cin >> temp;

    // find tail
    ListElement *tail = this->findTail();

    // search through list and if match, remove node
    for (ListElement *i = this->head; i != NULL; i = i->next)
    {
        if (temp == counter)
        {
            // reconnect list
            if (i == this->head)
            {
                // NULL->B->C
                // make C new head
                head = i->next;

                // NULL->C
                i->next->prev = NULL;
            }
            else if (i == tail)
            {
                // A->B->NULL
                // A->NULL
                i->prev->next = NULL;
            }
            else
            {
                //(A->B->C)
                // A->C
                i->prev->next = i->next;
                // C->A
                i->next->prev = i->prev;
            }

            // set B pointers to null and release B
            i->next = i->prev = NULL;
            delete i;

            // break out of structure
            break;
        }
        counter++;
    }
}
ListElement *DoubleLinkedList ::findTail()
{
    // go through nodes and find end of list
    ListElement *i;
    for (i = this->head; i->next != NULL; i = i->next)
        ;

    return i;
}