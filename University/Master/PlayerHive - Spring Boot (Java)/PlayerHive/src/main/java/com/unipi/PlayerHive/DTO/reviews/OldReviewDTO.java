package com.unipi.PlayerHive.DTO.reviews;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.bson.types.ObjectId;
import org.springframework.data.mongodb.core.mapping.Field;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class OldReviewDTO {

    @Field("review_id")
    private ObjectId reviewId;

    @Field("user_id")
    private ObjectId userId;

}
