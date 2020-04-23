package com.example.miniproject8_iot

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.google.firebase.database.*
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    lateinit var db: DatabaseReference

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        db = FirebaseDatabase.getInstance().getReference();
        val listener = object: ValueEventListener{
            override fun onCancelled(p0: DatabaseError) {

            }

            override fun onDataChange(p0: DataSnapshot) {
                var hasilWater = p0.child("Node1/WaterHeight").getValue().toString()
                waterValue.text =hasilWater

                var hasilRFID = p0.child("Node1/RFIDcode").getValue().toString()
                RFIDValue.text = hasilRFID
            }

        }
        db.addValueEventListener(listener)
    }
}
